# rvm

A tiny, fast, portable, and embeddable low-level virtual machine.

*`rvm` is now complete!*

## Usage

1. Make sure you have `make` and `gcc`/`clang` installed.
2. Clone the repo.
3. Run `make -C'path/to/repo'`

**For the APIs**: Please read the source code. Check out `test/test.c` to see
how to set it up. I don't plan to write a full documentation, yet. Thanks for
understanding.

## Benchmarks

I've got no access to a PC. RVM was entirely built on my phone, which has
these decent specs:

- processor: Unisoc  T603
- cpu: 8 cores (4x 1.2GHz + 4x 1.8GHz)
- arch: armv8 (aarch64), little-endian
- ram: 4GB (+4GB swap)

| Test           | Elapsed time | Inst Count          | Avg TPI | Inst Rate  |
| :------------: | :----------: | :-----------------: | :-----: | :--------: |
| `nops`         | 3.651 ms     | 1048576 (1.05 M)    | 3.482ns | 287.2M IPS |
| `cnt_1bill`    | 9.509 secs   | 1000000006 (1.01 B) | 8.317ns | 105.2M IPS |
| `naive_fib_40` | 18.662 secs  | 2980442527 (2.98 B) | 6.262ns | 159.7M IPS |

CI benchmarks (GitHub-hosted runner):

| Test           | Elapsed time | Inst Count          | Avg TPI | Inst Rate  |
| :------------: | :----------: | :-----------------: | :-----: | :--------: |
| `nops`         | 456.413 μs   | 1048576 (1.05 M)    | 0.435ns | 2.297G IPS |
| `cnt_1bill`    | 2.904 secs   | 1000000006 (1 B)    | 2.904ns | 344.4M IPS |
| `naive_fib_40` | 4.459 secs   | 2980442527 (2.98 B) | 1.496ns | 668.4M IPS |

In the naive `fib(40)` test, RVM is ~2.5× slower than LuaJIT &mdash; but
**orders of magnitude faster** than Python!

## Contributing

I'd appreciate any contributions! To contribute:

1. Fork the repo
2. Clone your fork
3. Make a new branch:

    ```sh
    git checkout -b fix/somebug
    ```

4. Make your changes.
5. Commit and push.

    ```sh
    git commit -m "fix bug #123"
    git push
    ```

6. Open a pull request.

Then just sit back! I may not respond instantly, but I’m always open to
contributions. Thanks!

## License

&copy; 2024-2025 Vincent Yanzee J. Tan. This project is licensed under
[GNU GPL v3.0 or later](./COPYING).
