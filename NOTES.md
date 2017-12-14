# Notes

## Compiling and Running

You'll need a C++ compiler and the NaCl library.  Build NaCl with the `./do`
script.  It'll take about ten minutes.  Then update your symlinks to point to
the NaCl headers and libraries that were generated:
 - `src/nacl` should point to the `include` folder.
 - `lib/nacl` should point to the `lib` folder for your architecture.

You can then use the `make` targets provided to create some sample programs:
 - `make objects` to populate the `obj` directory with object files.
 - `make tests` to create all the sample programs.

The funnest demo is the client/server one.  Run `./server.out` in one terminal.
It'll listen on port 7783 (SPUD in telephone code).  Run `./client.out` in a few
other terminals.  They'll automatically connect to the server.  There's no
prompt, but you can type into any of the clients; once you hit enter, your
message is sent, encrypted, to the server, and then broadcast, also encrypted,
to all known clients (i.e. clients that have sent at least one message to the
server before).

```
[17:28:39 holt@Minerva spud]$ ./client.out
Hello?
[Client 59578]: Hello?
[Client 53374]: Hello?
Hey!
[Client 59578]: Hey!
[Client 53374]: Oh hey Alice.
[Client 53374]: What's up?
Not much. Bored.
[Client 59578]: Not much. Bored.
you seen Eve recently?
[Client 59578]: you seen Eve recently?
[Client 49218]: ...
[Client 53374]: ^^^ !!!
Figures...
[Client 59578]: Figures...
^C
```


## Implementation Differences

### Key Derivation

NaCl doesn't have a key derivation function built in, so I had to write my own.
It's basically the algorithm given by Krawczysk in "Cryptographic Extraction and
Key Derivation: The HKDF Scheme" (and as found on the Wikipedia page) with the
slight change of not concatenating an "index" counter in the potentially looped
HMAC call.  There's no need in our case 'cause we only generate single blocks.
 - https://eprint.iacr.org/2010/264.pdf
 - https://en.wikipedia.org/wiki/HKDF


### Authenticated Encryption

NaCl has authenticated encryption, but doesn't support the plaintext "additional
data" section available in some authenticated encryption schemes.  I needed this
to ensure that (plaintext) headers couldn't be modified without detection, so I
ended up doing it myself (a modified encrypt-then-mac scheme):
 - https://en.wikipedia.org/wiki/Authenticated_encryption#Encrypt-then-MAC

```
c = Encrypt(message, key)
a = MAC(header || c, key)
return a || header || c
```


### Message Keys

In the full specification, "message keys" aren't used directly. Instead, they're
run through a key derivation function to generate a message encryption key, a
message authentication key, and a nonce.  I chose to avoid this, and used the
message keys directly for both encryption and authentication, and used the
message sequence number as the nonce.
 - https://signal.org/docs/specifications/doubleratchet/#recommended-cryptographic-algorithms


## Bugs and Miscellany

### Chain Interruption

In its current state, a malicious packet can trigger an invalid ratchet of the
DH "root" chain, breaking future legitimate communication.  This is becuase the
current implementation doesn't obey this part of the spec:
 - https://signal.org/docs/specifications/doubleratchet/#decrypting-messages

> If an exception is raised (e.g. message authentication failure) then the
> message is discarded and changes to the state object are discarded. Otherwise,
> the decrypted plaintext is accepted and changes to the state object are stored.

Given how easy it is to forge a UDP packet, this is a blocker for use in the
real world.  The fix is relatively simple, but there may be performance problems
(see below).


### Efficiency

A single message may trigger quite a bit of computation.  In the worst case, it
could cause:
 - `MAX_SKIP` (currently hard-coded as eight) * 2 HMAC calculations as the recv
   chain ratchets forward, storing potentially skipped keys.
 - Two Diffie-Hellman calculations and two HKDF calculations as the root chain
   ratchets forward one step.
 - `MAX_SKIP` * 2 more HMAC calculations as the new recv chain ratchets forward,
   storing potentially skipped keys.

And these calculations all must be done _before_ you can tell if a message was
legitimate: the final output from the recv chain is what you use to authenticate
and decrypt.  This means that receiving a packet requires you to copy the entire
state of the connection (about half a kilobyte) and make your changes to the
copy, in case they were triggered by an invalid packet.

While I haven't done any timing, this seems like an awful lot of work to be
caused by a single UDP packet (especially since they're so easy to forge - see
above).


### Header Encryption

Header encryption (section 4 of the spec) is not currently implemented (it's
technically optional), but might solve the efficiency concerns above.  This adds
"header keys" (and "next header keys"), shared symmetric keys that only change
when the root key changes.
 - https://signal.org/docs/specifications/doubleratchet/#double-ratchet-with-header-encryption

Since the header is encrypted with authenticated encryption, detecting forged
packets is now a matter of two symmetric key decryptions: once with the current
header key, and once with the next header key if that fails (more may be
required if there are skipped message keys with other, older header keys).

This seems likely to be more efficient, but again, actual testing is required.


### Magic Cookies

One possible extension (not in the spec, but plausible in the context of UDP) is
the use of "magic cookies" when establishing connections.  In order for a new
connection to be established, the "client" (whoever initiates the connection)
sends the "server" (whoever gets initiated on) a `hello` message.  This doesn't
open a connection - the server simply responds with a verifiable cookie for the
client's address.  The client then sends a `connect` message with the cookie it
received in the previous step.  If the cookie is valid, the server creates a
connection.

This is used in DTLS to prevent malicious packets from causing "expensive
cryptographic calculations" on the server.  The idea is that the cookie is easy
for the server to calculate and verify, but unforgeable. An HMAC of the client's
IP address and port (and likely a timestamp) makes sure that the client actually
responds on the source address it claimed.
 - https://tools.ietf.org/html/rfc6347#section-4.2.1

Alternatively, skipping the cookie exchange but requiring a proof-of-work on the
first message would make it take significantly more investment to initiate a
connection, providing similar protection.


### UDP Addresses

For unknown reasons, the `sockaddr_in` values generated by the `Spud::Address`
constructor and by the POSIX `recvfrom` function differ in the `sin_family`
field (the values populated by `recvfrom` seem to have an extra flag set):

```
[14:47:43 holt@Minerva spud]$ ./client.out
Send Addr: 00021e677f0000010000000000000000
hello
Recv Addr: 10021e677f0000010000000000000000
> hello
```

This means that the compare function is a bit more complicated than it needs to
be (ideally we could use `memcmp`).  Not a big deal.  Just weird.
