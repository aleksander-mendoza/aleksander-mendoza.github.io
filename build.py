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
    shutil.rmtree(gen)
if args.index:
    def genIndex(dir,url_prefix):
        dirName, subdirList, _ = next(os.walk(dev))
        for subdir in subdirList:
            subdir = dirName + '/' + subdir
            suburl = url_prefix + '/' + subdir
            print(subdir)
            print(suburl)
            with open(subdir + ".html", "w+") as file:
                for subsubdir in next(os.walk(subdir))[1]:
                    subsuburl = suburl + '/' + subsubdir
                    file.write("["+subsubdir+"]("+subsuburl+".html)")
            genIndex(subdir, suburl)
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
