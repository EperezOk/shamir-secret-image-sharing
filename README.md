# (k, n) secret image sharing scheme capable of cheating detection

This implementation is based on the paper located in `/docs/paper.pdf`.

> In a (k, n) threshold secret image sharing scheme, a secret image is encrypted into n image-shadows that satisfy the
following: (1) any less than k image-shadows get no information on the image and (2) any k or more image-shadows
can reconstruct the entire image. Cheating problem is an important issue in traditional secret sharing scheme.
However, this issue has not been discussed sufficiently in the field of secret image sharing. In this paper, we consider
the scenario of cheating behavior in secret image sharing scheme and construct a (k, n) threshold secret image
sharing scheme which is capable of cheating detection. Our proposed scheme is able to detect the cheating behavior
from up to k − 1 cheaters, and the size of image-shadow is almost same as the image-shadow in the original secret
image sharing scheme.

# Compile and run the program

To get started, open your terminal and run the following commands:

1. Compile the project:

```bash
make
```

This will generate an `ss` executable in the root directory, which you can use to hide or recover a secret `.bmp` image.

2. Distribute `secret.bmp` into `6` images located in the `covers` directory:

```bash
./ss d secret.bmp 6 covers
```

3. Recover the secret into `recovered.bmp` from the `6` images located in the `covers` directory:

```bash
./ss r recovered.bmp 6 covers
```

If you followed the previous steps, you should see that `secret.bmp` and `recovered.bmp` are _very similar_ (some white pixels might have turned black; see `docs/report.pdf` for more details on why this might happen).

# Run tests

To run all the tests for the project, you first need to install the [Criterion](https://github.com/Snaipe/Criterion) library.

For `Ubuntu (>=21.04)` and `Debian (>=11)`, you can install it with:

```
apt install libcriterion-dev
```

Once you have installed the library, just run:

```
make test
```

# Clean up

To clean up the generated binaries and objects, use:

```bash
make clean
```
