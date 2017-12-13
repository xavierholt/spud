# Implementation Notes

## On Key Derivation

NaCl doesn't have a key derivation function built in, so I had to write my own.
It's basically the algorithm given by Krawczysk in "Cryptographic Extraction and
Key Derivation: The HKDF Scheme" (and as found on the Wikipedia page) with the
slight change of not concatenating an "index" counter in the potentially looped
HMAC call.  There's no need in our case 'cause we only generate single blocks.
 - https://eprint.iacr.org/2010/264.pdf
 - https://en.wikipedia.org/wiki/HKDF


## On Authenticated Encryption

NaCl has authenticated encryption, but doesn't support the plaintext "additional
data" section available in some authenticated encryption schemes.  I needed this
to ensure that (plaintext) headers couldn't be modified without detection, so I
ended up doing it myself (a modified encrypt-then-mac scheme):

```
c = Encrypt(message, key)
a = MAC(header || c, key)
return a || header || c
```


