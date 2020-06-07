use sqlite;
use criterion::{criterion_group, criterion_main, Criterion};
use criterion::*;

fn fibonacci(n: u64) -> u64 {
    match n {
        0 => 1,
        1 => 1,
        n => fibonacci(n-1) + fibonacci(n-2),
    }
}

fn criterion_benchmark(c: &mut Criterion) {
    let connection = sqlite::open(":memory:").unwrap();
    connection.execute("CREATE TABLE users (name TEXT, age INTEGER);").unwrap();

    c.bench_function("create table/drop", move |b| b.iter_batched(|| &connection, |con|
        {
        con.execute(
            "REPLACE INTO users (name, age) VALUES('Test', 140000);")
            .unwrap();
        con.execute(
            "REPLACE INTO users (name, age) VALUES('Test', 240000);")
            .unwrap();
        }, BatchSize::SmallInput,
    ));
}

criterion_group!(benches, criterion_benchmark);
criterion_main!(benches);
