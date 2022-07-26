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

## Q3

MultiSig稍微修改。我这里使用的是先验证bank，然后再验证任意一个customer，出现的问题是`scriptSig`的压栈顺序，`bank_sig`应该最后压入，而`cust_sig`先压入，注意还需要补一个`OP_0`/`OP_FALSE`。

## Q4

参考：

- https://en.bitcoin.it/wiki/Contract#Example_5:_Trading_across_chains

- https://en.bitcoin.it/wiki/Atomic_swap

我理解的原理为：

Alice设置一个随机数$X$，然后取$Hash(X)$。设置两个tx，tx1是把钱给Bob（条件为提供$X$和Bob的签名，或者提供Alice和Bob两个人的签名），tx2是把钱给Alice自己（双花）但是locktime为48小时，即48小时之后才能使用，条件是用Alice和Bob两个人的签名。Bob先在Alice的tx2上签好名，于是Alice可以让tx2生效但不能让tx1生效。

Bob也同样设置tx1和tx2（sender和receiver对应改变），但是tx2的locktime为24小时。Alice也在Bob的tx2上签好名，于是Bob也可以让tx2生效，Alice能让Bob的tx1生效。

于是Alice就使用$X$和Alice的签名让Bob的tx1生效，于是Bob知道了$X$，就可以用$X$和Bob的签名让Alice的tx1生效。

出现的问题：

- 该库要求最后stack中必须有一个元素，true/false，所以最后一个OP不需要加VERIFY

- CHECKMULTISIG的时候需要看压入的顺序