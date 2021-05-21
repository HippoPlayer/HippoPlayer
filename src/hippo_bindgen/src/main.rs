use full_moon;
use full_moon::{ast::punctuated::Pair, ast::*, tokenizer::*, visitors::Visitor};
use std::fs;

#[derive(Debug, Default)]
pub struct TableEntry {
    pub line: usize,
    pub name: String,
    pub str_data: String,
    num: Option<i64>,
}

#[derive(Debug, Default)]
pub struct TableData {
    pub entries: Vec<TableEntry>,
}

/*
#[derive(Debug, Default)]
struct ParseIndex {
    name: String,
    table: TableData,
}

#[derive(Debug, Default)]
struct Flag {
    name: String,
    bits: u64,
    shift: u64,
    range: u64,
    base: u64,
    is_const: bool,
    index_list: Vec<ParseIndex>,
}
*/

/// Holds an entry and a comment with line and pos start
#[derive(Debug, Default, Clone)]
pub struct Comment {
    /// Position on the line (starting with 1)
    pub pos: usize,
    /// The actual data
    pub text: String,
}

/// A visitor that logs every local assignment made
#[derive(Default)]
pub struct GatherComments {
    /// Comments
    pub comments: Vec<Comment>,
    /// Each line with code will be marked as true
    pub codelines: Vec<bool>,
}

/// Holds an entry and a comment for it
#[derive(Debug, Default)]
pub struct EntryLine {
    /// the entry on the line
    pub text: String,
    /// line number of the data
    pub line: usize,
    /// Comment(s) for the line/belonging lines
    pub comment: String,
}

/// Arugments for functions/structs
#[derive(Debug, Default)]
pub struct FuncArg {
    /// Name and line
    pub name_line: EntryLine,
    /// Type of the argument
    pub type_name: String,
    /// Table data (such as out and other modifiers)
    pub table: Option<TableData>,
}

/// Holds data for a func/struct.x
#[derive(Debug, Default)]
pub struct Func {
    /// Comment(s) before the function
    pub comments: String,
    /// name of the function
    pub name: EntryLine,
    /// name of the class (like func.VertexBuffer this would be VertexBuffer)
    pub class: EntryLine,
    /// Table data for "settings" of the function
    pub table: TableData,
    /// Return type of the function
    pub return_type: EntryLine,
    /// function arguments
    pub args: Vec<FuncArg>,
}

/// Arugments for functions/structs
#[derive(Debug, Default)]
pub struct FlagArg {
    /// Name and line
    pub name_line: EntryLine,
    /// actual value of the flag. Overriden if there is a table
    pub value: Option<u64>,
    /// Table data (such as out and other modifiers)
    pub table: Option<TableData>,
}

/// Holds data for a flag/enum
#[derive(Debug, Default)]
pub struct Flag {
    /// Type in bits (such as 64, 32, 16, 8)
    pub size: usize,
    /// Comment(s) before the function
    pub comments: String,
    /// name of the function
    pub name: EntryLine,
    /// Table data for "settings" of the flag
    pub table: Option<TableData>,
    /// entries for the flag
    pub entries: Vec<FlagArg>,
}

/// Holds all the parsed data
#[derive(Debug, Default)]
pub struct Idl {
	/// Functions
	pub functions: Vec<Func>,
	/// Structs
	pub structs: Vec<Func>,
	/// Functions
	pub enums: Vec<Flag>,
	/// Structs
	pub flags: Vec<Flag>,
}

/// Holds flag attributes to calculate the flag values
#[derive(Debug, Default)]
struct FlagAttributes {
    bits: u64,
    shift: u64,
    range: u64,
    base: u64,
    is_const: bool,
}

fn get_identifier<'a>(token_ref: &'a TokenReference) -> Option<&'a str> {
    match token_ref.token().token_type() {
        TokenType::Identifier { identifier } => Some(identifier),
        _ => None,
    }
}

fn get_token_number<'a>(token_ref: &'a TokenReference) -> Option<&'a str> {
    match token_ref.token().token_type() {
        TokenType::Number { text } => Some(text),
        _ => None,
    }
}

fn get_token_string(token: &TokenReference) -> EntryLine {
    let mut entry = EntryLine::default();

    match token.token().token_type() {
        TokenType::StringLiteral {
            literal,
            multi_line: _,
            quote_type: _,
        } => entry.text = literal.to_owned().to_string(),
        TokenType::Identifier { identifier } => entry.text = identifier.to_owned().to_string(),
        _ => {
            panic!();
        }
    }

    entry.line = token.token().start_position().line();
    entry
}


fn get_token_var_string(var: &Var) -> EntryLine {
    match var {
        Var::Name(name) => get_token_string(name),
        _ => panic!(),
    }
}

fn get_value(entry: &mut TableEntry, value: &Value) {
    match value {
        Value::Number(value) => {
            let val = get_token_number(value).unwrap();
            // handle hex data
            if val.starts_with("0x") {
                entry.num = Some(i64::from_str_radix(&val[2..], 16).unwrap());
            } else {
                entry.num = Some(val.parse().unwrap());
            }

            entry.line = value.token().start_position().line();
        }
        Value::String(value) => {
            let t = get_token_string(value);
            entry.str_data = t.text;
            entry.line = t.line;
        }
        _ => (),
    }
}

fn get_expression(entry: &mut TableEntry, expr: &Expression) {
    match expr {
        Expression::Value { value } => get_value(entry, value),
        _ => (),
    }
}

fn get_string_value<'a>(value: &'a Value) -> EntryLine {
    match value {
        Value::String(value) => get_token_string(value),
        Value::Var(var) => get_token_var_string(var),
        _ => {
            dbg!(value);
            panic!();
        }
    }
}

fn get_expression_literal<'a>(expr: &'a Expression) -> EntryLine {
    match expr {
        Expression::Value { value } => get_string_value(value),
        _ => EntryLine::default(),
    }
}

fn get_table_data(table_data: &TableConstructor) -> Option<TableData> {
    let mut data = TableData::default();

    for t in table_data.fields() {
        match t {
            Field::NameKey {
                key,
                equal: _,
                value,
            } => {
                let mut entry = TableEntry::default();
                entry.name = get_identifier(key).unwrap().to_owned();
                get_expression(&mut entry, value);

                data.entries.push(entry);
            }

            Field::NoKey(expr) => {
                let mut entry = TableEntry::default();
                let t = get_expression_literal(expr);
                entry.name = t.text;
                entry.line = t.line;
                data.entries.push(entry);
            }

            _ => (),
        }
    }

    Some(data)
}

fn get_par_data(expr: &Expression) -> Option<TableData> {
    let mut table = TableData::default();
    let mut table_entry = TableEntry::default();
    get_expression(&mut table_entry, expr);

    if table_entry.str_data != "" || table_entry.num.is_some() {
        table_entry.name = "paran".to_owned();
        table.entries.push(table_entry);
        Some(table)
    } else {
        None
    }
}

fn fn_handle_anon_call<'a>(args: &'a FunctionArgs<'a>) -> Option<TableData> {
    match args {
        FunctionArgs::TableConstructor(table) => get_table_data(table),
        FunctionArgs::Parentheses {
            parentheses: _,
            arguments,
        } => {
            if let Some(t) = arguments.last() {
                match t {
                    Pair::End(t) => get_par_data(t),
                    _ => None,
                }
            } else {
                None
            }
        }
        _ => None,
    }
}

fn handle_call<'a>(call: &'a Call) -> Option<TableData> {
    match call {
        Call::AnonymousCall(args) => fn_handle_anon_call(args),
        _ => None,
    }
}

/*
fn get_index<'a>(index: &'a Index) -> String {
    match index {
        Index::Dot { dot: _, name } => get_token_string(name).unwrap().to_owned(),
        Index::Brackets {
            brackets: _,
            expression,
        } => {
            let mut entry = TableEntry::default();
            get_expression(&mut entry, expression);
            entry.str_data
        }
        _ => String::new(),
    }
}
*/

/*
fn get_func_args<'a>(func: &'a FunctionCall) -> Option<&'a FunctionArgs<'a>> {
    for (iter_i, s) in func.suffixes().enumerate() {
        dbg!(iter_i);
        match s {
            Suffix::Call(call) => handle_call(call),
            Suffix::Index(index) => {
                dbg!(index);
                //None
            }
            _ => (),
        }
    }

    None
}
*/

/*
fn get_flag(output: &mut u64, entry: &TableEntry, name: &str) {
    if entry.name == name {
        *output = entry.num.unwrap() as u64;
    }
}
*/

/*
fn generate_flags(func: &FunctionCall) -> Flag {
    let mut current_index = ParseIndex::default();
    let mut flag = Flag::default();

    for s in func.suffixes() {
        match s {
            Suffix::Call(call) => {
                println!("{} ------------------------------", current_index.name);
                dbg!(call);
                if current_index.table.entries.is_empty() {
                    current_index.table = handle_call(call).unwrap();
                }
            }
            Suffix::Index(index) => {
                if current_index.name != "" {
                    flag.index_list.push(current_index);
                }

                current_index = ParseIndex::default();
                current_index.name = get_index(index);
            }

            _ => (),
        }
    }

    if current_index.name != "" {
        flag.index_list.push(current_index);
    }

    let entry = &flag.index_list[0];
    flag.name = entry.name.to_owned();
    flag.shift = 0;

    for f in &entry.table.entries {
        if f.name == "const" {
            flag.is_const = true;
        }

        get_flag(&mut flag.bits, &f, "bits");
        get_flag(&mut flag.base, &f, "base");
        get_flag(&mut flag.range, &f, "range");
        get_flag(&mut flag.shift, &f, "shift");
    }

    flag
}
*/

fn get_prefix_identifier<'a>(prefix: &'a Prefix) -> Option<&'a str> {
    match prefix {
        Prefix::Name(name) => match name.token().token_type() {
            TokenType::Identifier { identifier } => Some(identifier),
            _ => None,
        },
        _ => None,
    }
}

/// Check if current suffix is a table
fn get_table(s: &Suffix) -> Option<TableData> {
    match s {
        Suffix::Call(call) => handle_call(call),
        _ => None,
    }
}

/// Get .<id/name> and also handle the special case of name["string"]
fn get_dot_name(s: &Suffix) -> EntryLine {
    match s {
        Suffix::Index(index) => match index {
            Index::Dot { dot: _, name } => get_token_string(name),
            Index::Brackets {
                brackets: _,
                expression,
            } => get_expression_literal(expression),
            _ => EntryLine::default(),
        },
        _ => EntryLine::default(),
    }
}

/// Get string as arg
fn get_arg_string(s: &Suffix) -> EntryLine {
    match s {
        Suffix::Call(call) => match call {
            Call::AnonymousCall(args) => match args {
                FunctionArgs::String(token) => get_token_string(token),
                _ => EntryLine::default(),
            },
            _ => EntryLine::default(),
        },
        _ => EntryLine::default(),
    }
}

/// Get the start comment line for a top entry (struct, func, enum, etc)
/// This works by working backwards from the starting line until no comment
/// was found. The comments also has to be on the very left edge (pos 1)
fn get_top_start_comment_lines(gather_com: &mut GatherComments, line_start: usize) -> String {
    let line_start = line_start - 1;
    let mut line = line_start;
    let mut comment = String::new();

    loop {
        // break if no text on line or the comment doesn't start on entry 1
        if line == 0 || gather_com.comments[line].pos != 1 || gather_com.comments[line].text == "" {
            break;
        }

        line -= 1;
    }

    for l in line + 1..=line_start {
        comment.push_str(&gather_com.comments[l].text);
        comment.push('\n');
    }

    comment
}

/// Get comments for the lines. The way this works is that it will search forward
/// from the current line until it finds a marked codeline or a line that starts
/// at the pos 1 (which is a function/struct/enum comment)
fn get_comments(entry: &mut EntryLine, gather_com: &mut GatherComments) {
	let mut comment = String::new();
	let mut line = entry.line;
	let max_len = gather_com.comments.len();

	loop {
		let entry = &gather_com.comments[line];

		if entry.text != "" {
			comment.push_str(&entry.text);
			comment.push('\n');
		}

		line += 1;

		if line == max_len {
			break;
		}

		let entry = &gather_com.comments[line];

		if gather_com.codelines[line] || entry.pos == 1 {
			break;
		}
	}

	entry.comment = comment;
}

/// Add comments to the function from the comments-prepass
fn update_comments(func: &mut Func, gather_com: &mut GatherComments) {
    // we start with the getting the comments for the function
    func.comments = get_top_start_comment_lines(gather_com, func.name.line);

	// all the lines that is being used by code so we know the ranges of
	gather_com.codelines[func.return_type.line] = true;

	for arg in &func.args {
		gather_com.codelines[arg.name_line.line] = true;
	}

	// get comments for the lines
	get_comments(&mut func.return_type, gather_com);

	for arg in &mut func.args {
		get_comments(&mut arg.name_line, gather_com);
	}
}

// get bits from the table useable data
fn get_bits_from_table(table: &TableData) -> FlagAttributes {
    let mut attribs = FlagAttributes::default();

	for f in &table.entries {
		match f.name.as_str() {
			"const" => attribs.is_const = true,
			"bits" => attribs.bits = f.num.unwrap() as u64,
			"base" => attribs.base = f.num.unwrap() as u64,
			"range" => attribs.range = f.num.unwrap() as u64,
			"shift" => attribs.shift = f.num.unwrap() as u64,
			_ => (),
		}
	}

	attribs
}

/// Parse a enum/flag. A function can be of the following formats:
fn parse_enum_or_flag(in_func: &FunctionCall, gather_com: &mut GatherComments, is_flag: bool) -> Flag {
    enum State {
        Name,
        Table,
        Arg,
    }

    let mut flag = Flag::default();
    let mut state = State::Name;
    let mut attribs = FlagAttributes::default();
    let mut value = 0u64;
    let mut arg = FlagArg::default();

    for s in in_func.suffixes() {
    	match state {
    		State::Name => {
    			flag.name = get_dot_name(s);
    			state = State::Table;
    		},

    		State::Table => {
    			let table = get_table(s);
    			if is_flag {
					if let Some(table) = table.as_ref() {
						attribs = get_bits_from_table(&table);
					    value = attribs.base << attribs.shift;
					} else {
						panic!("flag.{} doesn't have array of bit settings.", flag.name.text);
					}
    			}

    			flag.table = table;
    			state = State::Arg;
    		},

    		State::Arg => {
    			let arg_table = get_table(s);

				// check if data is a table and do it's setup
    			if let Some(table) = arg_table.as_ref() {
        			if table.entries[0].name == "paran" {
						let v = table.entries[0].num.unwrap() as u64;
						let v = if attribs.is_const || v == 0 {
							v
						} else {
							1u64 << (v - 1)
						};

						arg.value = Some(v);
					} else {
						arg.value = None;
						arg.table = arg_table;
					}
					flag.entries.push(arg);
					arg = FlagArg::default();
    			} else {
    				if !arg.name_line.text.is_empty() {
						flag.entries.push(arg);
						arg = FlagArg::default();
					}

					arg.name_line = get_dot_name(s);
					arg.value = Some(value);

					if attribs.shift != 0 || !is_flag {
						value += 1u64 << attribs.shift;
						dbg!(value);
					} else {
						value <<= 1;

						if value == 0 {
							value = 1;
						}
					}
    			}
    		},
    	}
    }

	// make sure to add the last arg if we have any
    if !arg.name_line.text.is_empty() {
    	flag.entries.push(arg);
    }

    flag
}

/// Parse a function/struct. A function can be of the following formats:
///
/// func.<StructName>.<funcname>
/// "return value"
/// .param "type" { extra data}
///
/// func.<funcname>
/// "return value"
///
fn parse_func_or_struct(in_func: &FunctionCall, gather_com: &mut GatherComments, skip_class_ret: bool) -> Func {
    enum State {
        ReturnType,
        ArgName,
        ArgType,
        MaybeTable,
    }

    let mut func = Func::default();
    let mut iter = in_func.suffixes();

    let name_or_class = get_dot_name(iter.next().unwrap());
    let mut it = iter.next().unwrap();
    let name = get_dot_name(it);

    // If we found an extension name we need to advance the iterator, otherwise assume the current
    // iterator is the return type or attributes for the function
    if !name.text.is_empty() && !skip_class_ret {
        func.class = name_or_class;
        func.name = name;
        it = iter.next().unwrap();
    } else {
        func.name = name_or_class;
        func.class.text.clear();
    }

    // Get table/attribute data for the function
    if let Some(table) = get_table(it) {
        func.table = table;
    }

    // if we found a table we need to advance the iterator again
    if !func.table.entries.is_empty() {
        it = iter.next().unwrap();
    }

    let mut arg = FuncArg::default();
    let mut state = if skip_class_ret {
    	State::ArgName
    } else {
    	State::ReturnType
    };

    loop {
        match state {
            State::ReturnType => {
                func.return_type = get_arg_string(it);
                state = State::ArgName;
            }
            State::ArgName => {
                arg.name_line = get_dot_name(it);
                state = State::ArgType;
            }

            State::ArgType => {
                let arg_type = get_arg_string(it);
                arg.type_name = arg_type.text;
                state = State::MaybeTable;
            }

            State::MaybeTable => {
				arg.table = get_table(it);

				if arg.table.is_some() {
                    func.args.push(arg);
                    arg = FuncArg::default();
                    state = State::ArgName;
                } else {
                    // if this wasn't a table its the first arg of the next argument.
                    func.args.push(arg);
                    arg = FuncArg::default();
                    arg.name_line = get_dot_name(it);
                    state = State::ArgType;
                }
            }
        }

        match iter.next() {
            Some(val) => it = val,
            None => break,
        };
    }

    if arg.name_line.text != "" {
        func.args.push(arg);
    }

    // Update comments
    update_comments(&mut func, gather_com);

    func
}

/*
fn gen_bits(f: &Flag) {
    println!("bitflags! {{");
    println!("    struct {}: u{} {{", f.name, f.bits);

    let mut value = f.base << f.shift;

    for e in f.index_list.iter().skip(1) {
        if e.table.entries.is_empty() {
            println!("           const {} = {:#016x};", e.name, value);
        } else if e.table.entries[0].name == "paran" {
            let v = e.table.entries[0].num.unwrap() as u64;
            let v = if f.is_const || v == 0 {
                v
            } else {
                1u64 << (e.table.entries[0].num.unwrap() - 1)
            };
            println!("           const {} = {:#016x};", e.name, v);
        } else {
            print!("           const {} = ", e.name);

            for t in &e.table.entries {
                print!("Self::{}.bits | ", t.name);
            }

            println!("0;");
        }

        if f.shift != 0 {
            value += 1u64 << f.shift;
        } else {
            value <<= 1;

            if value == 0 {
                value = 1;
            }
        }
    }

    if f.shift > 0 {
        println!("           const Shift = {};", f.shift);
    }

    if f.range > 0 {
        let v = if f.range == 64 {
            0xffffffffffffffff
        } else {
            ((1u64 << f.range) - 1) << f.shift
        };
        println!("           const Mask = {:#016x};", v);
    }

    println!("    }}");
    println!("}}\n");
}
*/

/*
/// Generate all the flags
fn print_flags(flags: &Vec<Flag>) {
    for f in flags {
        gen_bits(&f);
    }
}

fn gen_enums(f: &Flag) {
    println!("#[repr(C)]");
    println!("pub enum {} {{", f.name);

    for e in f.index_list.iter().skip(1) {
        println!("    {},", e.name);
    }

    println!("}}\n");
}
*/

/*
/// Generate enumes
fn print_enums(enums: &Vec<Flag>) {
    for f in enums {
        gen_enums(&f);
    }
}
*/

/*
struct VertexLayout {
    data: bgfx::bgfx_vertex_layout_s;
}

impl VertexLayout {
    pub fn new() -> VertexLayout {
        VertexLayout { data: bgfx::bgfx_vertex_layout_s::default(); }
    }

    pub fn begin(&self, render_type: RenderType) -> &self {
        unsafe { bgfx_vertex_layout_begin(self.data as *const void, render_type);
        self
    }

    pub fn add(attrib: Attrib, num: u8, type: AttribType, normalized: bool, as_int: bool) -> &self {
        unsafe { bgfx_vertex_layout_add(self.data as *const void, render_type);
    }

    pub fn has(attrib: Attrib) -> bool {
        unsafe { bgfx_vertex_layout_has(self.data as *const void) }
    }

    pub fn add(attrib: Attrib, num: u8, type: AttribType, normalized: bool, as_int: bool) -> &self {
        unsafe { bgfx_vertex_layout_add(self.data as *const void, render_type);
    }
}


*/

/*
fn generate_func(func: &Flag) {


}
*/

impl<'ast> Visitor<'ast> for GatherComments {
    fn visit_single_line_comment(&mut self, token: &Token<'ast>) {
        let text = match token.token_type() {
            TokenType::SingleLineComment { comment } => comment,
            _ => panic!(),
        };

        let pos = token.start_position();

        if text.starts_with("- ") {
			self.comments[pos.line()] = Comment {
				pos: pos.character(),
				text: text[2..].to_owned().to_string(),
			};

        } else {
			self.comments[pos.line()] = Comment {
				pos: pos.character(),
				text: text.to_owned().to_string(),
			};
        }
    }
}

fn main() -> Result<(), Box<dyn std::error::Error + 'static>> {
    let filename = "/home/emoon/temp/foo.idl";

    //let bgfx_defs: String = fs::read_to_string("../external/bgfx/scripts/bgfx.idl")?;
    let bgfx_defs: String = fs::read_to_string(filename)?;
    let lua_data = full_moon::parse(&bgfx_defs).unwrap();
    let line_count = bgfx_defs.as_bytes().iter().filter(|&&c| c == b'\n').count();

    let mut visitor = GatherComments {
        comments: vec![Comment::default(); line_count],
        codelines: vec![false; line_count],
    };

    // Gather all the comments
    visitor.visit_ast(&lua_data);

    // funcs, enums and structs
    let mut funcs = Vec::new();
    let mut structs = Vec::new();
    let mut enums = Vec::new();
    let mut flags = Vec::new();

    //let mut funcs = Vec::new();

    for node in lua_data.nodes().stmts() {
        match node {
            Stmt::FunctionCall(func) => {
                match get_prefix_identifier(func.prefix()) {
                    Some("flag") => flags.push(parse_enum_or_flag(func, &mut visitor, true)),
                    Some("enum") => enums.push(parse_enum_or_flag(func, &mut visitor, false)),
                    Some("struct") => structs.push(parse_func_or_struct(func, &mut visitor, true)),
                    Some("func") => funcs.push(parse_func_or_struct(func, &mut visitor, false)),
                    _ => (),
                }
            }
            _ => (),
        }
        //dbg!(node);
    }

    dbg!(enums);

    //dbg!(funcs);
    //dbg!(structs);

    //print_flags(&flags);
    //print_enums(&enums);

    //dbg!(funcs);

    Ok(())
}
