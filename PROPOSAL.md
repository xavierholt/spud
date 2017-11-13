# Proposal: SPUD

A datagram encryption scheme based on the Double Ratchet Algorithm (formerly the
Axolotl Ratchet) by Trevor Perrin and Moxie Marlinspike.


## References

 - The official spec: <https://signal.org/docs/specifications/doubleratchet/>
 - Wikipedia: <https://en.wikipedia.org/wiki/Double_Ratchet_Algorithm>
 - The original spec: <https://github.com/trevp/double_ratchet/wiki>


## Overview

I'm interested in distributed, decentralized systems like peer-to-peer networks.
In these systems, large numbers of nodes exchange intermittent messages. Typical
network protocols -  JSON over HTTP, for example - don't serve particularly well
here.  Nodes are expected to join or leave the network at random, so TCP isn't a
great fit, particularly considering that I expect most messages to fit in single
packets.

UDP makes a lot more sense here, in my opinion,  but there isn't an overwhelming
consensus on how to encrypt UDP traffic (as there is with TCP and TLS). The most
common approach seems to be to use [DTLS][1],  which is an extension of [TLS][2]
that can deal with the quirks of datagram traffic: out-of-order delivery, repeat
packets, dropped packets, etc.

But  DTLS  comes with strong ties to the  CA  system that it  inherits from TLS,
something I'd like to avoid.  I prefer  to have cryptographic keys  as a part of
one's identity.  In (D)TLS, you connect to a server, and then ask that server to
prove its identity.  I would much rather get the address  and public key  of the
server as a single unit  (call it cryptographic-identity-addressable networking,
perhaps) and then connect with no questions asked.

So I'd like to try something different.  The Double Ratchet Algorithm  was first
proposed to protect messages in secure chat applications,  but it applies nicely
to  datagram traffic.  Messages  (packets)  might get lost.  One party might not
respond for a while. Messages might even get lost, or delivered out of order. If
the protocol can handle this for text messages, it can handle if for UDP.


## Details

The  Double Ratchet Algorithm  works by maintaining what it calls "KDF Chains" -
series of keys derived from one another with extra data added as necessary.  The
general operation looks like:

```
Chain Key 1 >-\   |-------|   /--> Chain Key 2
               |->| K D F |>-|
Other Data >--/   |-------|   \--> Derived Key
```

The chain keys  are enver used  except to derive  other chain keys.  The derived
keys are used to encrypt messages.  If two parties agree on an initial Chain Key
Zero and the Other Data is a shared constant,  they can use this construction to
generate a series of one-time encryption/decryption keys.

In  the Double Ratchet Algorithm,  each party maintains two key chains,  one for
sending  and one for receiving.  The derived keys from these chains are used for
symmetrically encrypting messages.

There is also a third "root" chain. As the parties send messages back and forth,
they piggyback Diffie-Hellman key exchange data on their real messages. Whenever
a new DH secret is agreed upon,  it is used as Other Data  to increment the root
chain.  The  derived key from this operation  is used to refresh the send or the
receive chain (it alternates).

The result of all of this  is that an attacker  who manages to find any of these
keys has a short-lived advantage. A message key is only used to encrypt a single
message,  and a  chain key  will be refreshed  by a new  DH secret  within a few
exchanges.  This is the "self-healing" property of the algorithm that lead to it
initially being named after the [axolotl][3], a salamander able to regrow limbs.


## Proposal

I would like to implement  the algorithm  in C  or C++.  I am most interested in
encrypting UDP or other datagram traffic,  but an ideal implementation would act
as a  "datagram stream wrapper"  that could encrypt  and forward  packets to any
transport backend.

Random thoughts:
 - All  connections should be initialized by a client talking to a server with a
   known DH (or ECDH) public key.   This lets us use a  shared secret to encrypt
   even the first message.
 - I may forgo the [recommended cryptographic algorithms][4] and instead use the
   [NaCl][5] library.  I've always wanted to use it.




[1]: https://en.wikipedia.org/wiki/Datagram_Transport_Layer_Security
[2]: https://en.wikipedia.org/wiki/Transport_Layer_Security
[3]: https://en.wikipedia.org/wiki/Axolotl
[4]: https://signal.org/docs/specifications/doubleratchet/#recommended-cryptographic-algorithms
[5]: https://en.wikipedia.org/wiki/NaCl_(software)
