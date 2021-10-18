import glob

for name in glob.glob("**/*.cpp", recursive=True):
    print(name.replace("\\", "/"))