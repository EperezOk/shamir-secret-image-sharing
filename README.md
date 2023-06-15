# (k, n) secret image sharing scheme capable of cheating detection

- `/images` contains a secret image, with `k = 6`.

## Compile and run the program

To get started, open your terminal and run the following commands:

1. Compile the project:

```bash
make
```

This will generate an `ss` executable in the root directory.

2. Run the executable:

```bash
./ss images/Albertshare.bmp 3 covers
```

## Run tests

To run all the tests for the project, you first need to install the [Criterion](https://github.com/Snaipe/Criterion) library.

For `Ubuntu (>=21.04)` and `Debian (>=11)`, you can install it with:

```
apt install libcriterion-dev
```

Once you have installed the library, just run:

```
make test
```

## Clean up

To clean up the generated binaries, use:

```bash
make clean
```
