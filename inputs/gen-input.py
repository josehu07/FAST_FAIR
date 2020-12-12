import sys
import random


def gen_monotonic(num_keys):
    key = 0
    for i in range(num_keys):
        key += random.randint(1, 10000)
        print("2 "+str(key))

def gen_reverse(num_keys):
    keys = []
    key = 0
    for i in range(num_keys):
        key += random.randint(1, 10000)
        keys.append(key)
    for k in keys[::-1]:
        print("2 "+str(k))

def gen_uniform(num_keys):
    keys = random.sample(range(1, 2**30), num_keys)
    random.shuffle(keys)
    for i in range(num_keys):
        print("2 "+str(keys[i]))


def print_help_message():
    print("Usage: python3 gen-input.py MODE NUM_KEYS")
    print("  MODE      monotonic|reverse|uniform")
    print("  NUM_KEYS  number of keys, should <= 100000")

def main():
    if len(sys.argv) != 3:
        print_help_message()
        exit(1)

    mode = sys.argv[1]
    num_keys = int(sys.argv[2])

    if mode == "monotonic":
        gen_monotonic(num_keys)
    elif mode == "reverse":
        gen_reverse(num_keys)
    elif mode == "uniform":
        gen_uniform(num_keys)
    else:
        print("Error: unrecognized mode "+mode)


if __name__ == "__main__":
    main()
