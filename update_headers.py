
import os

author_line = " * @author Chamath Adithya (SOLVEO)\n"
root_dir = "examples"

for dirpath, dirnames, filenames in os.walk(root_dir):
    for filename in filenames:
        if filename.endswith(".ino"):
            filepath = os.path.join(dirpath, filename)
            with open(filepath, "r") as f:
                content = f.readlines()
            
            # Find the end of the first comment block
            updated = False
            for i, line in enumerate(content):
                if "*/" in line:
                    # Check if already added to avoid duplicates
                    if i > 0 and "@author" in content[i-1]:
                        print(f"Skipping {filename}, already has author.")
                        updated = True
                        break
                    
                    # Insert author line before the closing */
                    content.insert(i, author_line)
                    updated = True
                    print(f"Updated {filename}")
                    break
            
            if updated:
                with open(filepath, "w") as f:
                    f.writelines(content)
            else:
                print(f"Warning: Could not find header in {filename}")
