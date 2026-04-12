'''
Program to go from unstructured text -> tokens -> abstract syntax tree 
'''

COMMA   = 'comma'
NUMBER  = 'number'
CONTAIN = 'contain'
BAGS    = 'bags'
NO      = 'no'
OTHER   = 'other'
WORD    = 'word'
DOT     = 'dot'

def isdot(c):
    return c == '.'
def isspace(c):
    return c == ' '
def iscomma(c):
    return c == ','
def isword(c):
    return c.isalpha()
def isnumber(c):
    return c.isdigit()
def isdot(c):
    return c == '.' 

def find_token(ct):
    token_map = {'contain':CONTAIN, 'contains':CONTAIN, 'bag':BAGS, 'bags': BAGS, 'no':NO, 'other':OTHER}
    if ct in token_map.keys():
        return token_map[ct]
    elif isnumber(ct):
        return NUMBER
    elif isword(ct):
        return WORD
    return None 

class Token:
    def __init__(self, value, ttype):
        self.token_value = value 
        self.token_type = ttype

    def __repr__(self):
        return f"{self.token_value}:{self.token_type}"

def lex(statement):
    p = 0
    tokens = [] 
    current_token = "" 
    while p < len(statement):
        c = statement[p]
        is_delim = iscomma(c) or isdot(c) or isspace(c)
        if is_delim:
            possible_token = find_token(current_token)
            if (possible_token):
                tokens.append(Token(current_token, possible_token))
                current_token = ""
                              
            if iscomma(c):
                tokens.append(Token(c, COMMA)) 
            
            if (isdot(c)):
                tokens.append(Token(c, DOT))

        if not is_delim:
            current_token += c
        p += 1
                              
    if len(current_token) > 0:
        possible_token = find_token(current_token)
        if possible_token:
            tokens.append(Token(current_token, possible_token))
                          
    return tokens

class Parser:
    def __init__(self, tokens):
        self.tokens = tokens
        self.pos = 0
        self.n = len(tokens)

    def peek(self):
        if self.pos < self.n:
            return self.tokens[self.pos]
        else:
            return None

    def consume(self, expected_type):
        t = self.peek() 
        if t and t.token_type == expected_type:
            self.pos += 1
            return t
        else:
            actual = t.token_type if t else 'EOF' 
            raise SyntaxError(f"Expected {expected_type}, found {actual}") 
    def parse_bag_name(self):
        word1 = self.consume(WORD);
        word2 = self.consume(WORD);
        return f"{word1.token_value} {word2.token_value}"

    def parse_child(self):
        number_tok = self.consume(NUMBER)
        bag_name = self.parse_bag_name()
        self.consume(BAGS)
        return {'count': int(number_tok.token_value), 'name': bag_name}

    def parse_child_list(self):
        children = [self.parse_child()] 
        while self.peek() and self.peek().token_type == COMMA:
            self.consume(COMMA)
            children.append(self.parse_child())
        return children

    def parse_contents(self):
        if self.peek() and self.peek().token_type == NO:
            self.consume(NO)
            self.consume(OTHER)
            self.consume(BAGS)
            self.consume(DOT)
        else:
            children = self.parse_child_list()
            self.consume(DOT)
            return children
        return [] 
            
    
    def parse(self):
        parent = self.parse_bag_name()
        self.consume(BAGS)
        self.consume(CONTAIN)

        children = self.parse_contents()
        return {parent:children} 
        
l = lex("dark orange bags contain 3 bright white bags, 4 muted yellow bags.")
p = Parser(l)
print(p.parse())
