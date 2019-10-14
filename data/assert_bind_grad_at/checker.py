import sys
import collections

def isclose(a, b, rel_tol=1e-4, abs_tol=1e-4):
    return abs(a-b) <= max(rel_tol * max(abs(a), abs(b)), abs_tol)

def compare_print(src_print_list, tgt_print_list):
    if len(src_print_list) == 0:
        return True
    def list2dict(print_list):
        all_print = {}
        for _print in print_list:
            tensor = _print.split(" = ")[0].split(": ")[1]
            value = float(_print.split(" = ")[1])
            if tensor in all_print:
                all_print[tensor][1] += 1
            else:
                all_print[tensor] = [value, 1]
        return all_print
    src_all_print = list2dict(src_print_list)
    tgt_all_print = list2dict(tgt_print_list)

    if set(src_all_print.keys()) != set(tgt_all_print.keys()):
        return False
    else:
        for tensor in tgt_all_print:
            if (not isclose(tgt_all_print[tensor][0], src_all_print[tensor][0])) or (tgt_all_print[tensor][1] != src_all_print[tensor][1]):
                return False
        return True


def compare(once_eval_src, once_eval_tgt):
    try:
        if len(once_eval_src) != len(once_eval_tgt):
            return False
        elif len(once_eval_src) == 0 and len(once_eval_tgt) == 0:
            return True
        else:
            Error = False
            for ele in once_eval_tgt:
                if "ERROR" in ele:
                    Error = True
                    break
            if Error:
                try:
                    assert len(once_eval_tgt) == 1
                except:
                    return False
                tgt_list = once_eval_tgt[0].split("|||")
                if once_eval_src[0] in tgt_list: return True
                else: return False
            else:
                print(once_eval_src, once_eval_tgt, file=sys.stderr)
                if compare_print(once_eval_src[:-1], once_eval_tgt[:-1]) and \
                    isclose(float(once_eval_src[-1]), float(once_eval_tgt[-1]), rel_tol=1e-4, abs_tol=1e-4):
                    return True
                else:
                    return False
    except:
        return False

def check(source_file, target_file):
    correct_flag, linenum, info = True, 0, "ok"
    once_eval_tgt, once_eval_src = [], []
    with open(source_file, "r") as fsrc:
        with open(target_file, "r") as ftgt:
            for tgt_line in ftgt:
                tgt_line = tgt_line.strip()
                if "----" in tgt_line:
                    #print(once_eval_src, once_eval_tgt)
                    if not compare(once_eval_src, once_eval_tgt):
                        correct_flag = False

                        if len(once_eval_tgt) > 0 and len(once_eval_src) > 0:
                            if once_eval_tgt[-1] != once_eval_src[-1]:
                                tgt, src = once_eval_tgt[-1], once_eval_src[-1]
                            else:
                                tgt, src = once_eval_tgt[0], once_eval_src[0]
                        else:
                            tgt = once_eval_tgt[0] if len(once_eval_tgt) > 0 else ""
                            src = once_eval_src[0] if len(once_eval_src) > 0 else ""
                        
                        info = "Wrong at line %d: \"%s\" expected, but \"%s\" found" % (linenum, tgt, src)
                        break
                    once_eval_tgt, once_eval_src = [], []
                else:
                    linenum += 1
                    src_line = fsrc.readline().strip()
                    once_eval_src.append(src_line)
                    once_eval_tgt.append(tgt_line)

            if info == 'ok':
                src = fsrc.readline().strip()
                if src != "" :
                    info = "Wrong at line %d: EOF exoected, but \"%s\" found" % (linenum, src)
                    correct_flag = False

    print(info, file=sys.stderr)
    return correct_flag

check(sys.argv[2], sys.argv[3])
