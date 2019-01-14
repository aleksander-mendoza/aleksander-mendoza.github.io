#!/usr/local/bin/python3
import os
import subprocess
import shutil
from argparse import ArgumentParser

parser = ArgumentParser()
parser.add_argument("--clean", dest="clean", default=False, action="store_true",
                    help="clean generated files")
parser.add_argument("--index", dest="index", default=False, action="store_true",
                    help="generate index tables")
parser.add_argument("--gen", dest="gen", default=False, action="store_true",
                    help="generate actual html")
parser.add_argument("--all", dest="all", default=False, action="store_true",
                    help="full build")
args = parser.parse_args()
dev = 'dev'
gen = 'blog'
if args.all:
    args.clean = args.index = args.gen = True
if args.clean:
    if os.path.isdir(gen):
        shutil.rmtree(gen)
    for entry in os.walk('.'):
        for file in entry[2]:
            if file == ".DS_Store":
                toRemove = entry[0] + "/.DS_Store"
                print("removing: " + toRemove)
                os.remove(toRemove)
if args.index:
    def genIndex(dir,url_prefix,depth=1):
        dirName, subdirList, _ = next(os.walk(dir))
        parentName = os.path.basename(dirName)
        def fileToArticleName(fileName: str)->str:
            return fileName.replace('_', ' ')
        for subdirName in subdirList:
            subdir = dirName + '/' + subdirName
            suburl = url_prefix + '/' + subdirName
            print(subdirName + ": " + subdir + " --> " + suburl)
            with open(subdir + ".md", "w+") as file:
                _, subsubdirList, subsubfileList = next(os.walk(subdir))
                if depth==1:
                    file.write("[Hello world!](hello.html)\n\n")
                    file.write("[About mathematics!](mathematical.html)\n\n")
                    file.write("[About programming!](programming.html)\n\n")
                    file.write("[About machine learning!](machine_learning.html)\n\n")
                else:
                    file.write("Back: [" + fileToArticleName(parentName) + "](../"+parentName+".html)\n\n")
                file.write("Subsections:\n\n")
                for subsubdir in subsubdirList:
                    subsuburl = suburl + '/' + subsubdir
                    file.write("- ["+fileToArticleName(subsubdir)+"]("+ "../"*depth + subsuburl+".html)\n")
                file.write("\n\nArticles:\n\n")
                for subsubfile in subsubfileList:
                    if subsubfile.startswith("_"):
                        continue
                    if subsubfile.endswith(".md"):
                        subsubfilename = subsubfile[:-len(".md")]
                    if os.path.isdir(subdir + "/" + subsubfilename):
                        continue
                    subsuburl = suburl + '/' + subsubfilename
                    file.write("- ["+fileToArticleName(subsubfilename)+"]("+ "../"*depth + subsuburl+".html)\n")
            genIndex(subdir, suburl, depth+1)
    genIndex(dev, gen)

if args.gen:
    for dirName, subdirList, fileList in os.walk(dev):
        genDir = gen + dirName[len(dev):]
        if not os.path.exists(genDir):
            os.makedirs(genDir)
        for fname in fileList:
            if fname.endswith('.md'):
                htmlFile = fname[:-len('.md')] + '.html'
                with open(genDir+'/'+htmlFile, 'w+') as out:
                    subprocess.Popen(["markdown", dirName+'/'+fname], stdout=out)
