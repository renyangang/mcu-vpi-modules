package main

import (
	"C"
	"bufio"
	"encoding/binary"
	"fmt"
	"net"
	"time"
)
import "unsafe"

var (
	listener  net.Listener
	bufIn     []byte
	conn      net.Conn
	writeChan chan []byte
)

func init() {
	conn = nil
	writeChan = make(chan []byte, 10)
}

//export SetOutput
func SetOutput(data unsafe.Pointer, l C.int) {
	writeChan <- C.GoBytes(data, l)
}

//export GetInput
func GetInput() *C.char {
	return C.CString(string(bufIn))
}

//export StartServer
func StartServer(host *C.char, port *C.char) {
	// 监听端口
	listener, err := net.Listen("tcp", C.GoString(host)+":"+C.GoString(port))
	if err != nil {
		fmt.Println("Error starting TCP server:", err)
		return
	}
	fmt.Println("Server is listening on port " + C.GoString(port) + "...")

	go func() {
		for {
			if conn != nil {
				time.Sleep(time.Millisecond * 1)
				continue
			}
			// 接受连接
			conn, err := listener.Accept()
			if err != nil {
				fmt.Println("Error accepting connection:", err)
				continue
			}
			fmt.Println("Client connected:", conn.RemoteAddr())

			go handleRead(bufio.NewReader(conn))
			go handleWrite(bufio.NewWriter(conn))
		}
	}()
}

func handleWrite(writer *bufio.Writer) {
	sizeBuf := make([]byte, 4)
	for {
		bufOut := <-writeChan
		// fmt.Println("bufOut:", bufOut)
		binary.BigEndian.PutUint32(sizeBuf, uint32(len(bufOut)))
		writer.Write(sizeBuf)
		_, err := writer.Write(bufOut)
		if err != nil {
			fmt.Println("Error writing to client:", err)
			conn.Close()
			conn = nil
			return
		}
		writer.Flush()
	}

}

func handleRead(reader *bufio.Reader) {
	sizeBuf := make([]byte, 4)
	for {
		_, err := reader.Read(sizeBuf)
		if err != nil {
			fmt.Println("Error reading from client:", err)
			conn.Close()
			conn = nil
			return
		}
		// fmt.Println("sizeBuf:", sizeBuf)

		msgLen := binary.BigEndian.Uint32(sizeBuf)
		buf := make([]byte, msgLen)
		rLen := 0
		for rLen < int(msgLen) {
			n, err := reader.Read(buf[rLen:])
			if err != nil {
				fmt.Println("Error reading from client:", err)
				conn.Close()
				conn = nil
				return
			}
			rLen += n
		}
		bufIn = buf
	}
}

func main() {
	// StartServer(C.CString("127.0.0.1"), C.CString("8009"))
	// go func() {
	// 	bs := make([]byte, 4)
	// 	bs[0] = 1
	// 	bs[1] = 2
	// 	bs[2] = 3
	// 	bs[3] = 4
	// 	fmt.Println(bs)
	// 	for {
	// 		fmt.Println(bufIn)
	// 		time.Sleep(time.Second * 5)
	// 		writeChan <- []byte{3, 0, 0, 0, 2, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
	// 	}
	// }()
	// for {
	// 	time.Sleep(time.Second * 1)
	// }
}
