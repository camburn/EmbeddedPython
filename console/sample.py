"""
Our Python sample module
"""
import random


class Console:

    def __init__(self, lines):
        self.lines = lines

    def write(self, data):
        print(data)


def print_random_word(num=5):
    """ Generate a random word then print to screen.

    Args:
        num (int): Number of random letters in word to generate
    """
    some_string = ''.join(random.sample('abcdefghijklmnopqrstuvwyxz', num))
    print('Python says:', some_string)


def run_custom_c_module():
    import custom2

    instance = custom2.Custom("John", "James", 77)
    print(instance.name())

