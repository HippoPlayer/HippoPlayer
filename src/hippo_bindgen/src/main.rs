use full_moon;
use full_moon::{ast::punctuated::Pair, ast::*, tokenizer::*};
use std::fs;

#[derive(Debug, Default)]
struct TableEntry {
    name: String,
    str_data: String,
    num: Option<i64>,
}

type TableData = Vec<TableEntry>;

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

fn get_token_string<'a>(token_ref: &'a TokenReference) -> Option<&'a str> {
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

fn get_token_var_string<'a>(var: &'a Var) -> Option<&'a str> {
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
        }
        Value::String(value) => {
            let t = get_token_string(value).unwrap().to_owned();
            entry.str_data = t;
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

fn get_string_value<'a>(value: &'a Value) -> Option<&'a str> {
    match value {
        Value::String(value) => get_token_string(value),
        Value::Var(var) => get_token_var_string(var),
        _ => {
            dbg!(value);
            panic!();
        }
    }
}

fn get_expression_literal<'a>(expr: &'a Expression) -> Option<&'a str> {
    match expr {
        Expression::Value { value } => get_string_value(value),
        _ => None,
        //None,
    }
}

fn get_table_data(table_data: &TableConstructor) -> TableData {
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

                data.push(entry);
            }

            Field::NoKey(expr) => {
                let mut entry = TableEntry::default();
                entry.name = get_expression_literal(expr).unwrap().to_owned();
                data.push(entry);
            }

            _ => (),
        }
    }

    data
}

fn get_par_data(expr: &Expression) -> TableData {
    let mut table = TableData::new();
    let mut table_entry = TableEntry::default();
    get_expression(&mut table_entry, expr);

    if table_entry.str_data != "" || table_entry.num.is_some() {
        table_entry.name = "paran".to_owned();
        table.push(table_entry);
    }

    table
}

fn fn_handle_anon_call<'a>(args: &'a FunctionArgs<'a>) -> TableData {
    match args {
        FunctionArgs::TableConstructor(table) => get_table_data(table),
        FunctionArgs::Parentheses {
            parentheses: _,
            arguments,
        } => {
            if let Some(t) = arguments.last() {
                match t {
                    Pair::End(t) => get_par_data(t),
                    _ => TableData::default(),
                }
            } else {
                TableData::default()
            }
        }

        _ => TableData::default(),
    }
}

fn handle_call<'a>(call: &'a Call) -> TableData {
    match call {
        Call::AnonymousCall(args) => fn_handle_anon_call(args),
        _ => TableData::default(),
    }
}

fn get_index<'a>(index: &'a Index) -> Option<&'a str> {
    match index {
        Index::Dot { dot: _, name } => get_token_string(name),
        _ => None,
    }
}

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

fn get_flag(output: &mut u64, entry: &TableEntry, name: &str) {
    if entry.name == name {
        *output = entry.num.unwrap() as u64;
    }
}

fn generate_flags(func: &FunctionCall) -> Flag {
    let mut current_index = ParseIndex::default();
    let mut flag = Flag::default();

    for s in func.suffixes() {
        match s {
            Suffix::Call(call) => {
            	if current_index.table.is_empty() {
                	current_index.table = handle_call(call);
                }
            }
            Suffix::Index(index) => {
                if current_index.name != "" {
                    flag.index_list.push(current_index);
                }

                current_index = ParseIndex::default();
                current_index.name = get_index(index).unwrap().to_owned();
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

    for f in &entry.table {
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

fn get_prefix_identifier<'a>(prefix: &'a Prefix) -> Option<&'a str> {
    match prefix {
        Prefix::Name(name) => match name.token().token_type() {
            TokenType::Identifier { identifier } => Some(identifier),
            _ => None,
        },
        _ => None,
    }
}

fn handle_func_call(flags: &mut Vec<Flag>, func: &FunctionCall) {
    match get_prefix_identifier(func.prefix()) {
        Some("flag") => flags.push(generate_flags(func)),
        _ => (),
    }
}

fn gen_bits(f: &Flag) {

	if f.bits == 64 {
    	println!("    struct {}: u64 {{", f.name);
	} else if f.bits == 32 {
    	println!("    struct {}: u32 {{", f.name);
	} else if f.bits == 16 {
    	println!("    struct {}: u16 {{", f.name);
	} else {
    	println!("    struct {}: u8 {{", f.name);
	}

    let mut value = f.base << f.shift;

    for e in f.index_list.iter().skip(1) {
        if e.table.is_empty() {
            println!("           const {} = {:#016x};", e.name, value);
        } else if e.table[0].name == "paran" {
            let v = e.table[0].num.unwrap() as u64;
            let v = if f.is_const || v == 0 {
                v
            } else {
                1u64 << (e.table[0].num.unwrap() - 1)
            };
            println!("           const {} = {:#016x};", e.name, v);
        } else {
            print!("           const {} = ", e.name);

            for t in &e.table {
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
}

/*
fn gen_bits_32(f: &Flag) {
    println!("    struct {}: u32 {{", f.name);

    let mut value = f.base << f.shift;

    for e in f.index_list.iter().skip(1) {
        if e.table.is_empty() {
            println!("           const {} = {:#08x};", e.name, value);
        } else if e.table[0].name == "paran" {
            let v = e.table[0].num.unwrap() as u32;
            let v = if f.is_const || v == 0 { v } else { 1u32 << (v - 1) };
            println!("           const {} = {:#08x};", e.name, v);
        } else {
            print!("           const {} = ", e.name);

            for t in &e.table {
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

    if f.range > 0 {
        let v = if f.range == 32 {
            0xffffffff
        } else {
            ((1u64 << f.range) - 1) << f.shift
        };
        println!("           const Mask = {:#08x};", v);
    }
}
*/

/// Generate all the flags
fn print_flags(flags: &Vec<Flag>) {
    for f in flags {
        println!("bitflags! {{");

		gen_bits(&f);


        println!("    }}");
        println!("}}");
    }

    /*
    bitflags! {
        struct Flags: u32 {
            const A = 0b00000001;
            const B = 0b00000010;
            const C = 0b00000100;
            const ABC = Self::A.bits | Self::B.bits | Self::C.bits;
        }
    }
    */
}

fn main() -> Result<(), Box<dyn std::error::Error + 'static>> {
    //let bgfx_defs: String = fs::read_to_string("../external/bgfx/scripts/bgfx.idl")?;
    let bgfx_defs: String = fs::read_to_string("/home/emoon/temp/foo.idl")?;
    let lua_data = full_moon::parse(&bgfx_defs).unwrap();
    let mut flags = Vec::new();

    for node in lua_data.nodes().stmts() {
        match node {
            Stmt::FunctionCall(func) => {
                handle_func_call(&mut flags, func);
            }
            _ => (),
        }
        //dbg!(node);
    }

    print_flags(&flags);

    //dbg!(flags);

    Ok(())
}
