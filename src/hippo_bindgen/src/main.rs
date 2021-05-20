use full_moon;
use full_moon::{ast::punctuated::Pair, ast::*, tokenizer::*, visitors::Visitor};
use std::fs;

#[derive(Debug, Default)]
struct TableEntry {
    line: usize,
    name: String,
    str_data: String,
    num: Option<i64>,
}

#[derive(Debug, Default)]
struct TableData {
    entries: Vec<TableEntry>,
}

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

/// Holds an entry and a comment with line and pos start
#[derive(Debug, Default, Clone)]
struct Comment {
    /// Position on the line (starting with 1)
    pos: usize,
    /// The actual data
    text: String,
}

/// A visitor that logs every local assignment made
#[derive(Default)]
struct GatherComments {
    /// Comments
    comments: Vec<Comment>,
    /// Each line with code will be marked as true
    codelines: Vec<bool>,
}

/// Holds an entry and a comment for it
#[derive(Debug, Default)]
struct EntryLine {
    /// the entry on the line
    text: String,
    /// line number of the data
    line: usize,
}

/// Arugments for functions
#[derive(Debug, Default)]
struct FuncArg {
    /// Name and line
    name_line: EntryLine,
    /// Type of the argument
    type_name: String,
    /// Comment
    comment: String,
    /// Table data (such as out and other modifiers)
    table: TableData,
}

/// Holds data for a func.x
#[derive(Debug, Default)]
struct Func {
    /// Comment(s) before the function
    comments: String,
    /// name of the function
    name: EntryLine,
    /// name of the class (like func.VertexBuffer this would be VertexBuffer)
    class: EntryLine,
    /// Table data for "settings" of the function
    table: TableData,
    /// Return type of the function
    return_type: EntryLine,
    /// function arguments
    args: Vec<FuncArg>,
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

/*
fn get_token_string_temp<'a>(token_ref: &'a TokenReference) -> Option<&'a str> {
    match token_ref.token().token_type() {
        TokenType::StringLiteral {
            literal,
            multi_line: _,
            quote_type: _,
        } => Some(literal),
        TokenType::Identifier { identifier } => Some(identifier),
        _ => {
            panic!();
        }
    }
}
*/

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

/// Add comments to the function from the comments-prepass
fn update_comments(func: &mut Func, comments: &mut GatherComments) {
    // we start with the
    func.comments = get_top_start_comment_lines(comments, func.name.line);
}

/// Generate a function. A function can be of the following formats:
///
/// func.<StructName>.<funcname>
/// "return value"
/// .param "type" { extra data}
///
/// func.<funcname>
/// "return value"
///
fn generate_func(in_func: &FunctionCall, gather_com: &mut GatherComments) -> Func {
    enum FuncState {
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
    if !name.text.is_empty() {
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
    let mut state = FuncState::ReturnType;

    loop {
        match state {
            FuncState::ReturnType => {
                func.return_type = get_arg_string(it);
                state = FuncState::ArgName;
            }
            FuncState::ArgName => {
                arg.name_line = get_dot_name(it);
                state = FuncState::ArgType;
            }

            FuncState::ArgType => {
                let arg_type = get_arg_string(it);
                arg.type_name = arg_type.text;
                state = FuncState::MaybeTable;
            }

            FuncState::MaybeTable => {
                if let Some(table) = get_table(it) {
                    arg.table = table;
                    func.args.push(arg);
                    arg = FuncArg::default();
                    state = FuncState::ArgName;
                } else {
                    // if this wasn't a table its the first arg of the next argument.
                    func.args.push(arg);
                    arg = FuncArg::default();
                    arg.name_line = get_dot_name(it);
                    state = FuncState::ArgType;
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
fn handle_func_call(flags: &mut Vec<Flag>, enums: &mut Vec<Flag>, func: &FunctionCall) {
    match get_prefix_identifier(func.prefix()) {
        Some("flag") => flags.push(generate_flags(func)),
        Some("enum") => enums.push(generate_flags(func)),
        Some("func") => generate_func(func),
        _ => (),
    }
}
*/

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

/// Generate enumes
fn print_enums(enums: &Vec<Flag>) {
    for f in enums {
        gen_enums(&f);
    }
}

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

        self.comments[pos.line()] = Comment {
            pos: pos.character(),
            text: text.to_owned().to_string(),
        };
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

    //let mut funcs = Vec::new();

    for node in lua_data.nodes().stmts() {
        match node {
            Stmt::FunctionCall(func) => {
                match get_prefix_identifier(func.prefix()) {
                    //Some("flag") => flags.push(generate_flags(func)),
                    //Some("enum") => enums.push(generate_flags(func)),
                    Some("func") => funcs.push(generate_func(func, &mut visitor)),
                    _ => (),
                }
            }
            _ => (),
        }
        //dbg!(node);
    }

    dbg!(funcs);

    //print_flags(&flags);
    //print_enums(&enums);

    //dbg!(funcs);

    Ok(())
}
