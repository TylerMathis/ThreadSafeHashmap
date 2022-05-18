# wait-free-hashmap
Wait free parallel hashmap implemented in C++.

## Usage

Reference tests for usage examples.

## Style

Use CamelCase for all names. Start types (such as classes, structs, and typedefs) with a capital letter, other names (functions, variables) with a lowercase letter. You may use an all-lowercase name with underscores if your class closely resembles an external construct (e.g., a standard library construct) named that way.

## Testing

When adding new code, please supply tests in the `tests` directory in a file called `Test<feature>.cpp`, utilizing assertions for invariants. Run your tests with `make test`.

Testing description and requirements:
 - All tests must *fully* test all behavior and functionality of the structure being tested.
 - Examples can be found in the `tests` directory.

## Benchmarking

When adding new code, please supply sequential vs threaded benchmarks in the `benches` directory in a file called `Bench<feature>.cpp`, utilizing the `chrono` namepsace for timing utilities and reporting time in `ms`. Run your benchmarks with `make bench`.

Benchmarking description and requirements:
 - All benchmarks must must benchmark reasonable scenarios using wall-clock time, and output their results to a `csv` file in the `analysis/data` directory.
 - After completing a benchmark, please provide graphs that correctly describe your benchmarks in the analysis directory.
 - Benchmarks will be run and updated by a consistent machine before each official paper update for consistency.

## Data visualization

To spin up our benchmark visualizations, you will need a Conda installation. If you are unfamiliar with Conda, I recommend installing `miniconda`. Once installed, create a new virtual environment with `conda create -n <name>`. Then, you can install the visualization dependencies with `conda install --file analysis/conda_req.txt`. Finally, spin up a Jupyter Labs sessions with `jupyter-lab`, and open and run the `analysis/notebook.ipynb` to view visualizations.

## Research Paper
For more information on our research, take a look at our [research paper](https://drive.google.com/file/d/1yHDpvQzmfGA9zntTmVzWkE9dsDYwKQdy/view?usp=sharing) or our [class presentation](https://www.youtube.com/watch?v=sJyN7pZLt54)!
