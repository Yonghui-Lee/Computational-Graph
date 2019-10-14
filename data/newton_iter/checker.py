import sys
import collections

def isclose(a, b, rel_tol=1e-4, abs_tol=1e-4):
    return abs(a-b) <= max(rel_tol * max(abs(a), abs(b)), abs_tol)
def compare(src_line, tgt_line):
    try:
        src_num_list = list(map(float, src_line.split()))
        tgt_num_list = list(map(float, tgt_line.split()))
        if len(src_num_list) != len(tgt_num_list):
            return False
        correct_flag = True
        for src_num, tgt_num in zip(src_num_list, tgt_num_list):
            if not isclose(src_num, tgt_num):
                correct_flag = False
                break
        return correct_flag
    except:
        return False

def check(source_file, target_file):
    print(source_file, target_file)
    correct_flag, linenum, info = True, 0, "ok"
    with open(source_file, "r") as fsrc:
        with open(target_file, "r") as ftgt:
            for tgt_line in ftgt:
                linenum += 1
                tgt_line = tgt_line.strip()
                src_line = fsrc.readline().strip()
                #print(once_eval_src, once_eval_tgt)
                if not compare(src_line, tgt_line):
                    correct_flag = False
                    info = "Wrong at line %d: \"%s\" expected, but \"%s\" found" % (linenum, tgt_line, src_line)
                    break

            if info == 'ok':
                src = fsrc.readline().strip()
                if src != "" :
                    info = "Wrong at line %d: EOF exoected, but \"%s\" found" % (linenum, src)
                    correct_flag = False

    print(info, file=sys.stderr)
    return correct_flag

check(sys.argv[1], sys.argv[2])
