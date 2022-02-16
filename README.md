# wait-free-hashmap
Wait free parallel hashmap implemented in C++.

## Style

Use CamelCase for all names. Start types (such as classes, structs, and typedefs) with a capital letter, other names (functions, variables) with a lowercase letter. You may use an all-lowercase name with underscores if your class closely resembles an external construct (e.g., a standard library construct) named that way.

## Testing

When adding new code, please supply tests in a file called `Test<feature>.cpp`, utilizing assertions for invariants. Run your tests with `make test`.

## Benchmarking

When adding new code, please supply sequential vs threaded benchmarks in a file called `Bench<feature>.cpp`, utilizing the `chrono` namepsace for timing utilities and reporting time in `ms`. Run your benchmarks with `make bench`.
