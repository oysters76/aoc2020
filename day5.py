class Node:
    def __init__(self, start, end, left, right):
        self.start = start
        self.end = end
        self.left = left
        self.right = right
        
def expand(l):
    if len(l) == 1:
        x = l[0]
        m = x//2
        return [[0,m-1],[m,x-1]]
 
    result = []
    
    for row in l:
        start, end = row
        m = (start+end) // 2 
        r = [[start, m], [m+1, end]]
        for el in r:
            result.append(el) 

    return result

#t = [128]
#while True:
#    l = input() 
#    if (l == 'e'):
#        break
#    else:
#        t = expand(t)
#        print(t) 

def build_tree(val):
    root = Node(0, val-1, None, None)
    stack = [root]
    while len(stack) > 0:
        node = stack.pop()

        is_range_less = (node.end - node.start) <= 1
        if is_range_less:
            node.left = Node(node.start, node.start, None, None)
            node.right = Node(node.end, node.end, None, None)
            continue

        m = (node.start+node.end)//2 
        n1 = Node(node.start, m, None, None)
        n2 = Node(m+1, node.end, None, None)

        node.left = n1
        node.right = n2 

        stack.append(n1)
        stack.append(n2)

    return root 

#root = build_tree(128)
root = build_tree(8)
n = root
while True:
    print(n.start, n.end)
    d = input(">")
    if (n.left == None or n.right == None):
        break
    if (d == 'f'):
        n = n.left
    else:
        n = n.right
        
