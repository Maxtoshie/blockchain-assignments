CC = gcc
CFLAGS = -Wall -Wextra -O2 -Iinclude -I/usr/include/openssl
LDFLAGS = -lssl -lcrypto -lpthread -lm

TARGET = health_insurance
SRCS = src/main.c src/blockchain.c src/block.c src/transaction.c \
       src/mempool.c src/utxo.c src/account.c src/mining.c \
       src/cryptography.c src/insurance.c src/fraud.c \
       src/persistence.c src/cli.c src/utils.c
OBJS = $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET) data/*.bin data/*.dat

test: $(TARGET)
	./$(TARGET) < tests/test_scenario.txt

run: $(TARGET)
	./$(TARGET)

.PHONY: all clean test run
