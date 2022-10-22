from http.client import HTTPResponse
import socket


SERVER_ADDR = "0.0.0.0"
SERVER_PORT = 8000
SERVER_PORT2 = 8002
SERVER_NAME = "youpi"


request_header = "GET /auto HTTP/1.1\r\nHost: {}\r\n\r\n".format(SERVER_NAME)
client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
client.connect((SERVER_ADDR, SERVER_PORT))
client.send(request_header.encode())
# read and parse http response
http_response = HTTPResponse(client)
http_response.begin()
print(http_response.read().decode())

