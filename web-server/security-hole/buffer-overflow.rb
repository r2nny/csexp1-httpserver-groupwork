require 'socket'

def send_command(cmd)
  sock = TCPSocket.open('localhost', 10100)

  buf = "GET / HTTP/1.1\r\n"
  buf += "Host: localhost\r\n"
  buf += "User-Agent: Mozilla/5.0\r\n"
  buf += "Content-Length: 100\r\n"
  buf += cmd
  buf += "\r\n\r\n"

  sock.puts(buf)
  
  while line = sock.gets
    puts line
  end

  sock.close
end

puts "Enter a command to execute on the server:"
cmd = gets.chomp
send_command(cmd)
