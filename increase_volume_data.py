with open('shakespeare.txt', 'r+b') as f:
    f.seek(0)
    content = f.read()
    for i in range(90):
        f.write(content)