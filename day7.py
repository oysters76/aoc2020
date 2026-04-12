'''
Program to go from unstructured text -> tokens -> abstract syntax tree

output:
[light red]
  ├── 1x bright white
  │   └── 1x shiny gold
  │       ├── 1x dark olive
  │       │   ├── 3x faded blue
  │       │   └── 4x dotted black
  │       └── 2x vibrant plum
  │           ├── 5x faded blue
  │           └── 6x dotted black
  └── 2x muted yellow
      ├── 2x shiny gold
      │   ├── 1x dark olive
      │   │   ├── 3x faded blue
      │   │   └── 4x dotted black
      │   └── 2x vibrant plum
      │       ├── 5x faded blue
      │       └── 6x dotted black
      └── 9x faded blue
[dark orange]
  ├── 3x bright white
  │   └── 1x shiny gold
  │       ├── 1x dark olive
  │       │   ├── 3x faded blue
  │       │   └── 4x dotted black
  │       └── 2x vibrant plum
  │           ├── 5x faded blue
  │           └── 6x dotted black
  └── 4x muted yellow
      ├── 2x shiny gold
      │   ├── 1x dark olive
      │   │   ├── 3x faded blue
      │   │   └── 4x dotted black
      │   └── 2x vibrant plum
      │       ├── 5x faded blue
      │       └── 6x dotted black
      └── 9x faded blue
[bright white]
  └── 1x shiny gold
      ├── 1x dark olive
      │   ├── 3x faded blue
      │   └── 4x dotted black
      └── 2x vibrant plum
          ├── 5x faded blue
          └── 6x dotted black
[muted yellow]
  ├── 2x shiny gold
  │   ├── 1x dark olive
  │   │   ├── 3x faded blue
  │   │   └── 4x dotted black
  │   └── 2x vibrant plum
  │       ├── 5x faded blue
  │       └── 6x dotted black
  └── 9x faded blue
[shiny gold]
  ├── 1x dark olive
  │   ├── 3x faded blue
  │   └── 4x dotted black
  └── 2x vibrant plum
      ├── 5x faded blue
      └── 6x dotted black
[dark olive]
  ├── 3x faded blue
  └── 4x dotted black
[vibrant plum]
  ├── 5x faded blue
  └── 6x dotted black
[faded blue]
[dotted black]
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

class Graph:
    def __init__(self):
        self.bag_map = {}

    def add(self, d):
        for key in d.keys():
            children = d[key]
            if key not in self.bag_map.keys():
                self.bag_map[key] = {}
            for child in children:
                count, child_name = child["count"], child["name"]
                if not child_name in self.bag_map[key].keys():
                    self.bag_map[key][child_name] = 0
                self.bag_map[key][child_name] += count
                
    def visualize(self, start_node=None, indent="", visited=None):
        """
        Recursively prints the bag hierarchy starting from a specific node.
        If no start_node is provided, it prints all top-level bags.
        """
        if visited is None:
            visited = set()

        # If no starting point, find all roots (keys that exist in the map)
        if start_node is None:
            for root in self.bag_map.keys():
                print(f"[{root}]")
                self.visualize(root, "  ", visited)
            return

        # Avoid infinite recursion if the graph has cycles
        if start_node in visited:
            return
        visited.add(start_node)

        # Get children for the current node
        children = self.bag_map.get(start_node, {})
        
        for i, (child_name, count) in enumerate(children.items()):
            # Create a nice visual branch
            is_last = i == len(children) - 1
            connector = "└── " if is_last else "├── "
            
            print(f"{indent}{connector}{count}x {child_name}")
            
            # Recurse into the child if it has its own children
            next_indent = indent + ("    " if is_last else "│   ")
            if child_name in self.bag_map:
                self.visualize(child_name, next_indent, visited.copy())

prog = ['light red bags contain 1 bright white bag, 2 muted yellow bags.','dark orange bags contain 3 bright white bags, 4 muted yellow bags.','bright white bags contain 1 shiny gold bag.','muted yellow bags contain 2 shiny gold bags, 9 faded blue bags.','shiny gold bags contain 1 dark olive bag, 2 vibrant plum bags.','dark olive bags contain 3 faded blue bags, 4 dotted black bags.','vibrant plum bags contain 5 faded blue bags, 6 dotted black bags.','faded blue bags contain no other bags.','dotted black bags contain no other bags.']

graph = Graph()     

for st in prog:
    l = lex(st)
    p = Parser(l)
    graph.add(p.parse())
    
graph.visualize()
