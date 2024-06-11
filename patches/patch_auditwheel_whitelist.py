from os.path import join, dirname, abspath
import json

from auditwheel import policy

def patch_whitelisted_libs():
    policies = None

    with open(join(dirname(abspath(policy.__file__)), "manylinux-policy.json")) as f:
        policies = json.load(f)

    lib_whitelist = [
        # qt shared libraries dependencies
        "libxcb.so.1",

        # lua shared libraries
        "libluajit-5.1.so.2",
        "liblua-5.4.so.5",
        "liblua-5.3.so.5",
        "liblua-5.1.so.5",
    ]

    lib_blacklist = []

    changed = False

    for p in policies:
        for lib in lib_whitelist:
            if not (lib in p["lib_whitelist"]):
                p["lib_whitelist"].append(lib)
                changed = True

        for lib in lib_blacklist:
            if lib in p["lib_whitelist"]:
                p["lib_whitelist"].remove(lib)
                changed = True

    if changed:
        with open(join(dirname(abspath(policy.__file__)), "manylinux-policy.json"), "w") as f:
            f.write(json.dumps(policies))

if __name__ == '__main__':
    patch_whitelisted_libs()
