# seedxor

## Problem

You want to secure your crypto private key using Shamir's Secret Sharing. However, you want to prevent anyone who gains enough shares to be able to reconstruct the seed without a password or pass phrase.

## Solution

You can encode the seed using a pass phrase you select. Then use the encoded hexidecimal as the seed to generate the Shamir shares.

To reverse an recover the original seed, use the appropriate number of Shamir shares to recover your encoded seed. Use the xor tool to convert the encoded seed back to the original seed phrase using your pass phrase.

## Notes

This tool is intended to be used in conjunction with [Blockchain Common's seedtool-cli](https://github.com/BlockchainCommons/seedtool-cli).

## Example Usage

Use `seedtool-cli` to generate a random seed.
```
$ seedtool-cli
8b9c57f505292975ea095f5f0a0b46e0
```
Convert the seed to BIP39. This is the seed to use in your wallet.
```
$ seedtool-cli --in=hex --out=bip39
8b9c57f505292975ea095f5f0a0b46e0
merit tired wool apart myth road popular cloud gallery expect spider scatter
```
To generate backup, use `seedxor` to pass phase protect the seed.

```
$ ./seedxor
Enter password: foobar
Enter password again: foobar
Enter hexidecimal data: 8b9c57f505292975ea095f5f0a0b46e0
edf33897645b291385663d3e780b208f
```

Check what the new BIP39 looks like. This is what the Shamir will be based off of.
```
$ seedtool-cli --in=hex --out=bip39
edf33897645b291385663d3e780b208f
unknown oil chair silly raw mean better sibling dinosaur scare sight buyer
```

Use `seedtool-cli` to split the new seed into however many parts you would like.
```
$ seedtool-cli --in=hex --out=sskr --group=2-of-5
edf33897645b291385663d3e780b208f
tuna acid epic gyro jazz gems able acid able arch back onyx beta swan mild gift fact good unit belt ugly barn fuel beta back trip luau drum limp
tuna acid epic gyro jazz gems able acid acid luau flux join play cook zone tent jade dark wave king tuna gear mint bald vibe luck navy vast days
tuna acid epic gyro jazz gems able acid also also monk drop hope judo flux jolt monk redo ruby very user leaf zone aqua swan ruin vibe road gala
tuna acid epic gyro jazz gems able acid apex luck tent vibe zaps omit dice whiz solo surf lung miss unit silk dull acid cyan wand yank jury wand
tuna acid epic gyro jazz gems able acid aqua bald epic poem plus plus cusp bald inch need cola taco time bald drum city note belt zoom math twin
```

### Example Recovery

To recover your original seed, reverse the process.

Use `seedtool-cli` to recover the seed using the minimum number of required parts.
```
$ seedtool-cli --in=sskr
tuna acid epic gyro jazz gems able acid able arch back onyx beta swan mild gift fact good unit belt ugly barn fuel beta back trip luau drum limp
tuna acid epic gyro jazz gems able acid also also monk drop hope judo flux jolt monk redo ruby very user leaf zone aqua swan ruin vibe road gala
edf33897645b291385663d3e780b208f
```

Check what the BIP39 is for this seed.
```
$ seedtool-cli --in=hex --out=bip39
edf33897645b291385663d3e780b208f
unknown oil chair silly raw mean better sibling dinosaur scare sight buyer
```

Use `seedxor` with your password to convert the encoded seed back into the original seed.
```
$ ./seedxor
Enter password: foobar
Enter password again: foobar
Enter hexidecimal data: edf33897645b291385663d3e780b208f
8b9c57f505292975ea095f5f0a0b46e0
```

Finally, use `seedtool-cli` to convert the decoded seed back into the original BIP39 format to use for your wallet.
```
$ seedtool-cli --in=hex --out=bip39
8b9c57f505292975ea095f5f0a0b46e0
merit tired wool apart myth road popular cloud gallery expect spider scatter
```

### Example Recovery with wrong pass phrase

In the above example the pass phase used was `foobar`. Suppose instead you tried to recover the seed with `foobax`.

```
$ ./seedxor
Enter password: foobax
Enter password again: foobax
Enter hexidecimal data: 8b9c57f505292975ea095f5f0a0b46e0
edf338976451291385663d3e720b208f
```

This generates a different seed. Using `seedtool-cli` to convert to BIP39 shows an entirely different seed phrase.

```
$ seedtool-cli --in=hex --out=bip39
edf338976451291385663d3e720b208f
unknown oil chair silly barely mean better sibling dinosaur motion sight cabin
```
