# `epoll-echo-server`

An example of an echo server using epoll to respond to clients.

# Building

You may get builds from [here](https://github.com/AgentTroll/epoll-echo-server/releases/tag/1.0). Alternatively, you may build it yourself:

``` shell
git clone https://github.com/AgentTroll/epoll-echo-server.git
cd epoll-echo-server
cmake .
make
```

# Usage

Begin the server using:

``` shell
$ ./epoll-echo-server
```

Then you may open other terminals or use tmux to run clients:

``` shell
$ ./epoll-echo-client
```

# Credits

Built with [CLion](https://www.jetbrains.com/clion/)