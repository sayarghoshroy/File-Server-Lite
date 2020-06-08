# File Server Lite

### Basic file server and client using socket connections

<p align='justify'>
The client can access files stored on the server. 'listall' lists all files and directories hosted on the server. 'send' is used to download files from the server. All exceptions on the server side are handled.
</p>

> Usage: send \<filename\>

#### Example

```
> listall //client requests to view files hosted on the server
weather.txt
hi.png
hello.py
> send hi.png //client downloads 'hi.png'
```

#### To build the Server executable:

```bash
g++ -o file_server file_server.c
```

#### To build the Client executable:

```bash
g++ -o file_client file_client.c
```

- Precompiled executables have been provided.

#### Running an executable file 'exec':
```bash
./exec
```

- The server can be executed at a particular directory.

- Multiple clients running at various locations can access the server.

- No external libraries were used.

---
