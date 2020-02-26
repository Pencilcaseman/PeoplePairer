import random
# from Names import names

def generateNames(count):
    name = [0, 0, 0, 0, 0]
    res = []
    for i in range(count):
        nameStr = ""
        for char in name:
            nameStr += chr(65 + char)
        res.append(nameStr)

        name[-1] += 1

        for j in range(len(name) - 1, -1, -1):
            if name[j] > 25:
                name[j] = 0
                name[j - 1] += 1

    return res

names = generateNames(300)

def getOptions():
    count = len(names)
    res = []
    for i in range(count):
        if count % 100000 == 0:
            print("{}%".format((i + 1) / count * 100))
        
        new = []
        for j in range(5):
            val = random.randint(1, count)
            while val in new or val == i + 1:
                val = random.randint(1, count)
            new.append(val)
        res.append(new)

    return res

def printNameInfo():
    for i in range(len(names)):
        print("{}, {}".format(fullNames[i], i + 1))

def printOptionInfo():
    count = len(names)

    res = getOptions()
    
    for i in range(count):
        print(i + 1, end="")
        for j in range(5):
            print(", {}".format(res[i][j]), end="")
        print()

def writeNameInfo(directory="../namesAndNumbers.csv"):
    file = open(directory, "w")
    
    count = len(names)
    for i in range(count):
        if i % 1000 == 0:
            print("{}%".format((i + 1) / count * 100))
        
        file.write("{}, {}\n".format(names[i], i + 1))

    file.close()

def writeOptionInfo(directory="../numbersAndPreferences.csv"):
    file = open(directory, "w")
    
    count = len(names)

    res = getOptions()
    
    for i in range(count):
        if i % 1000 == 0:
            print("{}%".format((i + 1) / count * 100))
        
        file.write("{}".format(i + 1))
        for j in range(5):
            file.write(", {}".format(res[i][j]))
        file.write("\n")

    file.close()
