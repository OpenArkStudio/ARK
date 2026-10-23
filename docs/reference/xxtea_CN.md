# XXTEA 工具

`<ark/crypto/xxtea.hpp>` 通过 `ark::core` CMake 目标提供现有 XXTEA 格式，无需加载运行时插件。

```cpp
#include <string>
#include <ark/crypto/xxtea.hpp>

std::string ciphertext;
std::string plaintext;
if (ark::xxtea::xxtea_encrypt("message", "key", ciphertext)) {
    const bool decoded = ark::xxtea::xxtea_decrypt(ciphertext, "key", plaintext);
}
```

字符串重载成功时向输出追加结果，输入为空时返回 `false`。密钥最多使用 16 字节：短密钥在首个零字节处结束，剩余位置补零；恰好包含 16 个非零字节的密钥无需终止符；更长的密钥只使用前 16 字节。此规则保留现有密文格式。

原始指针重载要求密钥非空，指向带终止符的短密钥，或至少 16 个可读字节。成功时返回分配的内存，通过 `out_len` 返回长度，调用方使用 `std::free()` 释放结果。需要拥有型结果时优先使用字符串重载。原始指针接口的密钥、输入数据与长度指针均不能传入空指针。

`check-core` 覆盖精确长度分配的短密钥、完整 16 字节密钥、内嵌零字节、二进制往返及输出追加。AddressSanitizer 同时检查密钥处理是否读过短密钥的终止位置。
