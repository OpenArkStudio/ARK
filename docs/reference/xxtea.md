# XXTEA helper

`<ark/crypto/xxtea.hpp>` provides the existing XXTEA format through the `ark::core` CMake target. No runtime plugin is required.

```cpp
#include <string>
#include <ark/crypto/xxtea.hpp>

std::string ciphertext;
std::string plaintext;
if (ark::xxtea::xxtea_encrypt("message", "key", ciphertext)) {
    const bool decoded = ark::xxtea::xxtea_decrypt(ciphertext, "key", plaintext);
}
```

The string overloads append to the output on success and return `false` for empty input. Keys use at most 16 bytes: the first zero byte terminates a short key, and the remaining bytes are zero-filled. A key of exactly 16 nonzero bytes does not require a terminator. Longer keys use only their first 16 bytes. This normalization preserves the existing ciphertext format.

The raw overloads accept a non-null key pointing to either a terminated short key or at least 16 readable bytes. They return allocated storage, its length through `out_len`, and require the caller to release successful results with `std::free()`. Prefer the string overloads for owning results. Do not pass a null key, input pointer or length pointer to the raw interface.

`check-core` covers exact-size short-key allocations, full-width and embedded-zero keys, binary round trips and output appending. AddressSanitizer also checks that normalization does not read beyond a short key's terminator.
