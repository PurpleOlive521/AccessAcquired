import os 
from pathlib import Path

CONSOLE_TOOL_NAME = "CopyrightReplacementTool"

ReplacementCopyright = ""
CompatibleExtensionsFileName = "RunnableExtensions.txt"
CopyrightFileName = "ReplacementText.txt"
CompatibleFileTypes = []

def Main():
    # Directory of executable 
    ContainingFolderPath = Path.cwd().parent

    print(CONSOLE_TOOL_NAME, f": Running tool {CONSOLE_TOOL_NAME} on working directory '{ContainingFolderPath}'.")

    PopulateCompatibleFileTypes(CompatibleExtensionsFileName)
    Result = PopulateCopyright(CopyrightFileName)

    if(Result):
        FoundCompatibleFiles = FindCompatibleFiles(ContainingFolderPath)
        ReplaceCopyrightInFiles(FoundCompatibleFiles)
        print(CONSOLE_TOOL_NAME, f": Finished running tool {CONSOLE_TOOL_NAME} with status 'Success'.")
    else:
        print(CONSOLE_TOOL_NAME, f": Finished running tool {CONSOLE_TOOL_NAME} with status 'Failed'.")


def PopulateCompatibleFileTypes(FileName = ''):
    with open(FileName,'r') as OpenedFile:
        for line in OpenedFile:
            CompatibleFileTypes.append(line.strip())    

def FindCompatibleFiles(PathToSearchFrom = ''):

    CompatibleFiles = []

    for Root, Directories, Files in os.walk(PathToSearchFrom):

        # Don't try and modify the Tools own directory
        if(Root == str(Path.cwd())):
            continue

        for File in Files:
            FileName = os.fsdecode(File)

            FileNameComponents = FileName.split('.')

            if(len(FileNameComponents) > 1):
                FileExtension = FileNameComponents[len(FileNameComponents) - 1]
                if(CompatibleFileTypes.__contains__(FileExtension)):
                    CompatibleFiles.append(os.path.join(Root, File))
    
    return CompatibleFiles

def ReplaceCopyrightInFiles(FilesToModify = []):

    ModifiedCopyrights = 0

    for FileName in FilesToModify:

        FileContent = []
        with open(FileName,'r') as OpenedFile:

            FileContent = OpenedFile.readlines()
            if(len(FileContent) <= 0):
                continue

            LinesToRemove = 0
            for Line in FileContent:
                # Enforce copyright-first for all files, remove empty/redundant lines before it
                if(not Line.strip()):
                    LinesToRemove += 1
                    continue

                # Found existing copyright
                if(Line.strip().startswith("//")):
                    break
                # Found file content that isn't a copyright. Insert a dummyline to replace
                else:
                    FileContent.insert(LinesToRemove, "\n")
                    break

        # We always orient the content such that the first line is either the existing copyright, or a line safe to replace with the new copyright
        with open(FileName,'w') as OpenedFile:
            del FileContent[0:LinesToRemove]
            
            FileContent[0] = ReplacementCopyright
            OpenedFile.writelines(FileContent)

        ModifiedCopyrights += 1

    print(CONSOLE_TOOL_NAME, f": Modified {ModifiedCopyrights} copyrights in {len(FilesToModify)} files.")

def PopulateCopyright(FileName = ''):
    with open(FileName,'r') as OpenedFile:
        FirstLine = OpenedFile.readline()

        if(FirstLine.strip() != ""):
            global ReplacementCopyright 

            ReplacementCopyright = FirstLine

            return True
        
        return False

if __name__ == "__main__":
    Main()