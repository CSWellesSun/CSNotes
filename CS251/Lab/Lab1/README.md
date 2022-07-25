# 记录

由于区块一直没有confirm，所以后来就找不到交易了，因此主要看一下scriptPubKey和scriptSig即可。

## Q1

` PayToPublicKeyHash`的模板脚本。需要理解BTC的脚本机制：每个tx在`TxIn`中有`scriptSig`，在`TxOut`中`scriptPubKey`。当新产生一个tx的时候，需要设置BTC的来源tx，由此也就得到了来源tx的`scriptPubKey`，而这个新产生的tx有`scriptSig`，如果这两个脚本拼合在一起如果能执行为`true`，那么该tx在脚本层面合法。

注意：发送的交易output应该留一些作为手续费，否则无法被confirm(我一开始没有留)

`scriptSig: signature, my_public_key`

`scriptPubKey: OP_DUP, OP_HASH160, faucet_address, OP_EQUALVERIFY, OP_CHECKSIG`

## Q2

通过这个test发现自己对BTC交易存在的巨大误解，即BTC的交易实际上可以不指定sender或receiver，只要你的交易能够用脚本unlock之前的交易output，那么你就可以使用这笔UTXO里的钱。BTC钱包实际上是建立在更高层的应用，它自动在UTXO中检索能够使用的钱，作为自己的余额。

另外的错误在于写脚本的时候没有2DUP，因为ADD等操作都是会先pop出两个操作数，然后执行完的结果压入到栈内，因此必须要先2DUP一下。

还有一个错误在于EQUAL和EQUALVERIFY的区别。EQAUL弹出两个元素并比较是否相等然后压入true/false到栈内，而EQAULVERIFY还会再进行一次VERIFY，即查看栈顶是否是true，如果是true就直接**移除**，否则的话就报错，注意中间必须移除。最后结果为true的条件是栈为空或者留下一个元素且为true。

参考：《精通比特币(第二版)》

### Q2a

`scriptPubKey: OP_2DUP, OP_ADD, 32001, OP_EQUALVERIFY, OP_SUB, 2339, OP_EQUALVERIFY`

### Q2b

`scriptSig: 17170, 14831`