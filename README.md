# Required Libraries
[Openssl](https://github.com/openssl/openssl)

[NTL](https://github.com/libntl/ntl)

[Eigen](https://gitlab.com/libeigen/eigen)
# Build & Compile
Goto to folder ```PPSR/``` and execute

```
mkdir build
cd build
cmake ..
make
```
,which produces the binary executable file named ```main``` in ```build/```.

#### PCA.cpp can be used to perform dimensionality reduction on data.