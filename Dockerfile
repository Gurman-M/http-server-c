FROM ubuntu:latest

RUN apt-get update && apt-get install -y gcc && apt-get install -y make

# Create and set /http-server directory to the cwd
WORKDIR /http-server

# Copy project folder to container
COPY . .

# Compile and link the server code
RUN make -C src

# Expose the port for the server
EXPOSE 3490

# Start the server
CMD ["./src/server"]