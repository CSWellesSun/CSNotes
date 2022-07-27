# CryptoZombies

## L1

- `pragma solidity ^0.x.x;`注意后面有分号

- 支持`**`乘方

- 支持静态和动态数组。设置数组为`public`表示对别的APP可见，但不能被其他APP修改

- 默认函数属性为`public`，定义为`private`是好的习惯。

- 函数的其他属性：`returns (string)`返回值，`view`只读不写，`pure`甚至不访问应用中的数据（只使用传入的参数）

- contract里的event可以用app来监听事件：`YourContract.YourEvent(function(error, result){})`

- 获得数组的最后一位的index：`array.push() - 1`

## L2

- `Solidity`并不支持原生的字符串比较, 我们只能通过比较两字符串的`keccak256`哈希值来进行判断

- `Storage`变量是指永久存储在区块链中的变量。`Memory`变量则是临时的，当外部函数对某合约调用完成时，内存型变量即被移除。函数外部的状态变量默认为存储形式，而函数内部声明的变量的内存形式。

- 子类无法调用父类的`private`。`internal`如果某个合约继承自其父合约，这个合约即可以访问父合约中定义的“内部”函数；`external`这些函数只能在合约之外调用，它们不能被合约内的其他函数调用。

- 接口：先定义一个contract为`Interface`，然后在调用这个接口的contract中：`Interface interface = Interface(InterfaceAddress);`即可拿到这个接口。

## L3

- uint即uint256

- 返回值可以很多，用元组接收可以留空

- `OpenZeppelin`库的`Ownable`合约。

```solidity
/**
 * @title Ownable
 * @dev The Ownable contract has an owner address, and provides basic authorization control
 * functions, this simplifies the implementation of "user permissions".
 */
contract Ownable {
  address public owner;
  event OwnershipTransferred(address indexed previousOwner, address indexed newOwner);

  /**
   * @dev The Ownable constructor sets the original `owner` of the contract to the sender
   * account.
   */
  function Ownable() public {
    owner = msg.sender;
  }

  /**
   * @dev Throws if called by any account other than the owner.
   */
  modifier onlyOwner() {
    require(msg.sender == owner);
    _;
  }

  /**
   * @dev Allows the current owner to transfer control of the contract to a newOwner.
   * @param newOwner The address to transfer ownership to.
   */
  function transferOwnership(address newOwner) public onlyOwner {
    require(newOwner != address(0));
    OwnershipTransferred(owner, newOwner);
    owner = newOwner;
  }
}
```

- `modifier`中的`_;`表示从modifier中转入function中运行

- 节省gas的方法：将变量放到struct中，并且把相同类型的变量放在一起

- `now`返回1970.1.1以来的秒数(uint256，一般转为`uint32`尽管会发生“2038事件”)，`solidity`支持`years`/`weeks`/`days`/`hours`/`minutes`/`seconds`这些单位

- 传入结构体的时候需要指定`storage`存储指针

- `view`和`pure`从外部调用的时候不消耗gas，但是内部函数调用的时候会消耗gas

- `solidity`中遍历比`storage`便宜

## L4

- `this.balance`存储当前合约账户中的以太

- 在`contract`中添加`using SafeMath for uint256;`

- `require`错误的时候会把gas退回，而`assert`不会

- 注意`SafeMath`对uint256适用，而其他类型需要重新写

## L5

- 使用MetaMask的web3Provider
```html
<script language="javascript" type="text/javascript" src="web3.min.js"></script>
```

```javascript
window.addEventListener('load', function() {

  // 检查web3是否已经注入到(Mist/MetaMask)
  if (typeof web3 !== 'undefined') {
    // 使用 Mist/MetaMask 的提供者
    web3js = new Web3(web3.currentProvider);
  } else {
    // 处理用户没安装的情况， 比如显示一个消息
    // 告诉他们要安装 MetaMask 来使用我们的应用
  }

  // 现在你可以启动你的应用并自由访问 Web3.js:
  startApp()

})
```
- 需要地址和ABI来和合约对话
```html
<script language="javascript" type="text/javascript" src="foo_abi.js"></script>
```

- 实例化Web3.js
```javascript
// 实例化 myContract
var myContract = new web3js.eth.Contract(myABI, myContractAddress);
```

- web3.js和合约对话的两个函数`call`和`send`。`call`调用`view`和`pure`函数，例如`myContract.methods.myMethod(123).call()`；`send`需要消耗gas，例如`myContract.methods.myMethod(123).send()`

- 在合约中定义一个public变量的时候，自动添加了一个同名的函数，例如定义了一个数组`Zombie[] public zombies;`，那么可以这样调用`cryptoZombies.methods.zombies(id).call()`，此函数为异步函数，返回一个Promise
```javascript
function getZombieDetails(id) {
  return cryptoZombies.methods.zombies(id).call()
}

getZombieDetails(15)
.then(function(result) {
  console.log("Zombie 15: " + JSON.stringify(result));
});
```
- 使用MetaMask查看当前的账户
```javascript
var accountInterval = setInterval(function() {
    // 检查账户是否切换
    if (web3.eth.accounts[0] !== userAccount) {
    userAccount = web3.eth.accounts[0];
    // 调用一些方法来更新界面
    updateInterface();
    }
}, 100);
```
- 调用`send`函数。`sender`需要指定`from`，另外可以指定`gas`，不指定的话MetaMask会要求用户输入。
```javascript
function createRandomZombie(name) {
  // 这将需要一段时间，所以在界面中告诉用户这一点
  // 事务被发送出去了
  $("#txStatus").text("正在区块链上创建僵尸，这将需要一会儿...");
  // 把事务发送到我们的合约:
  return cryptoZombies.methods.createRandomZombie(name)
  .send({ from: userAccount })
  .on("receipt", function(receipt) {
    $("#txStatus").text("成功生成了 " + name + "!");
    // 事务被区块链接受了，重新渲染界面
    getZombiesByOwner(userAccount).then(displayZombies);
  })
  .on("error", function(error) {
    // 告诉用户合约失败了
    $("#txStatus").text(error);
  });
}
```
- `payable`传入的以太单位是wei，web3js的转换函数`web3js.utils.toWei("1", "ether");`，例如`cryptoZombies.methods.levelUp(zombieId)
.send({ from: userAccount, value: web3js.utils.toWei("0.001","ether") })`

- 监听事件。下面的代码中，一旦有一个zombie被创造都会触发提醒。
```solidity
event NewZombie(uint zombieId, string name, uint dna);
```
```javascript
cryptoZombies.events.NewZombie()
.on("data", function(event) {
  let zombie = event.returnValues;
  console.log("一个新僵尸诞生了！", zombie.zombieId, zombie.name, zombie.dna);
}).on('error', console.error);
```

- 使用`indexed`来过滤事件，之后就可以用`filter`。
```solidity
event Transfer(address indexed _from, address indexed _to, uint256 _tokenId);
```
```js
cryptoZombies.events.Transfer({ filter: { _to: userAccount } })
.on("data", function(event) {
  let data = event.returnValues;
  // 当前用户更新了一个僵尸！更新界面来显示
}).on('error', console.error);
```
- 查询过去的事件。用事件存储是一种更便宜的方法，一些之后不会使用的数据可以用event来存储。
```js
cryptoZombies.getPastEvents("NewZombie", { fromBlock: 0, toBlock: 'latest' })
.then(function(events) {
  // events 是可以用来遍历的 `event` 对象 
  // 这段代码将返回给我们从开始以来创建的僵尸列表
});
```
- 使用其他的Web3Provider
```js
var web3Infura = new Web3(new Web3.providers.WebsocketProvider("wss://mainnet.infura.io/ws"));
var czEvents = new web3Infura.eth.Contract(cryptoZombiesABI, cryptoZombiesAddress);
```

