import os
import subprocess
import tkinter as tk
from tkinter import ttk, messagebox, scrolledtext
from datetime import datetime

BASE_DIR = os.path.dirname(os.path.abspath(__file__))
ENGINE = os.path.join(BASE_DIR, "mini_sql.exe")
DATA_DIR = os.path.join(BASE_DIR, "data")


class MiniSQLEngineApp:
    def __init__(self, root):
        self.root = root
        self.root.title("Mini SQL Engine - University Project")
        self.root.geometry("1200x760")
        self.root.minsize(1000, 650)

        self.bg = "#0f172a"
        self.panel = "#111827"
        self.card = "#1e293b"
        self.editor_bg = "#020617"
        self.text = "#e5e7eb"
        self.muted = "#94a3b8"
        self.accent = "#38bdf8"
        self.green = "#22c55e"
        self.red = "#ef4444"

        self.root.configure(bg=self.bg)

        self.build_style()
        self.build_header()
        self.build_body()
        self.load_example()
        self.refresh_tables()

    def build_style(self):
        style = ttk.Style()
        style.theme_use("clam")

        style.configure(
            "Treeview",
            background=self.editor_bg,
            foreground=self.text,
            fieldbackground=self.editor_bg,
            rowheight=28,
            borderwidth=0
        )

        style.configure(
            "Treeview.Heading",
            background=self.card,
            foreground=self.text,
            font=("Segoe UI", 10, "bold")
        )

    def build_header(self):
        header = tk.Frame(self.root, bg=self.bg)
        header.pack(fill="x", padx=24, pady=(20, 10))

        title = tk.Label(
            header,
            text="Mini SQL Engine",
            font=("Segoe UI", 26, "bold"),
            bg=self.bg,
            fg=self.text
        )
        title.pack(side="left")

        subtitle = tk.Label(
            header,
            text="Flex + Bison + C + Python",
            font=("Segoe UI", 11),
            bg=self.bg,
            fg=self.muted
        )
        subtitle.pack(side="left", padx=18, pady=(9, 0))

        self.status = tk.Label(
            header,
            text="● Engine",
            font=("Segoe UI", 10, "bold"),
            bg=self.bg,
            fg=self.muted
        )
        self.status.pack(side="right", pady=10)

    def build_body(self):
        main = tk.Frame(self.root, bg=self.bg)
        main.pack(fill="both", expand=True, padx=24, pady=8)

        sidebar = tk.Frame(main, bg=self.panel, width=220)
        sidebar.pack(side="left", fill="y", padx=(0, 10))
        sidebar.pack_propagate(False)

        center = tk.Frame(main, bg=self.panel)
        center.pack(side="left", fill="both", expand=True)

        self.build_sidebar(sidebar)
        self.build_editor(center)

    def build_sidebar(self, parent):
        tk.Label(
            parent,
            text="DATABASE",
            font=("Segoe UI", 11, "bold"),
            bg=self.panel,
            fg=self.muted
        ).pack(anchor="w", padx=16, pady=(18, 8))

        self.tables = tk.Listbox(
            parent,
            bg=self.editor_bg,
            fg=self.text,
            selectbackground="#334155",
            selectforeground="white",
            borderwidth=0,
            highlightthickness=0,
            font=("Consolas", 10)
        )
        self.tables.pack(fill="x", padx=14, pady=(0, 10))

        tk.Button(
            parent,
            text="↻ Refresh Tables",
            command=self.refresh_tables,
            bg=self.card,
            fg=self.text,
            activebackground="#334155",
            activeforeground="white",
            relief="flat",
            cursor="hand2",
            pady=7
        ).pack(fill="x", padx=14)

        tk.Label(
            parent,
            text="COMMANDS",
            font=("Segoe UI", 11, "bold"),
            bg=self.panel,
            fg=self.muted
        ).pack(anchor="w", padx=16, pady=(24, 8))

        commands = [
            ("CREATE TABLE", "CREATE TABLE students (id INT, name TEXT, department TEXT, cgpa FLOAT);"),
            ("INSERT", "INSERT INTO students VALUES (1, 'Taiful', 'CSE', 3.50);"),
            ("SELECT", "SELECT * FROM students;"),
            ("SELECT WHERE", "SELECT name, cgpa FROM students WHERE department = 'CSE';"),
            ("UPDATE", "UPDATE students SET cgpa = 3.60 WHERE id = 1;"),
            ("DELETE", "DELETE FROM students WHERE id = 2;"),
            ("SHOW TABLES", "SHOW TABLES;"),
            ("DESCRIBE", "DESCRIBE students;"),
            ("DROP TABLE", "DROP TABLE students;")
        ]

        for name, sql in commands:
            tk.Button(
                parent,
                text=name,
                command=lambda value=sql: self.set_sql(value),
                bg=self.card,
                fg=self.text,
                activebackground="#334155",
                activeforeground="white",
                relief="flat",
                anchor="w",
                padx=10,
                pady=6,
                cursor="hand2"
            ).pack(fill="x", padx=14, pady=2)

        tk.Label(
            parent,
            text="\nCompiler Pipeline\n\n1. Lexer\n2. Parser\n3. Semantic check\n4. Execution\n5. File storage",
            justify="left",
            font=("Segoe UI", 9),
            bg=self.panel,
            fg=self.muted
        ).pack(anchor="w", padx=16, pady=15)

    def build_editor(self, parent):
        top = tk.Frame(parent, bg=self.panel)
        top.pack(fill="x", padx=18, pady=(18, 8))

        tk.Label(
            top,
            text="SQL QUERY EDITOR",
            font=("Segoe UI", 12, "bold"),
            bg=self.panel,
            fg=self.text
        ).pack(side="left")

        tk.Button(
            top,
            text="Run SQL",
            command=self.run_sql,
            bg=self.accent,
            fg="#082f49",
            activebackground=self.accent,
            relief="flat",
            font=("Segoe UI", 10, "bold"),
            padx=18,
            pady=7,
            cursor="hand2"
        ).pack(side="right", padx=(8, 0))

        tk.Button(
            top,
            text="Clear",
            command=lambda: self.editor.delete("1.0", "end"),
            bg=self.card,
            fg=self.text,
            activebackground="#334155",
            relief="flat",
            padx=15,
            pady=7,
            cursor="hand2"
        ).pack(side="right")

        self.editor = scrolledtext.ScrolledText(
            parent,
            height=12,
            wrap="none",
            bg=self.editor_bg,
            fg=self.text,
            insertbackground="white",
            font=("Consolas", 12),
            relief="flat",
            padx=12,
            pady=12
        )
        self.editor.pack(fill="x", padx=18)

        result_label = tk.Label(
            parent,
            text="RESULT / CONSOLE",
            font=("Segoe UI", 12, "bold"),
            bg=self.panel,
            fg=self.text
        )
        result_label.pack(anchor="w", padx=18, pady=(14, 7))

        self.output = scrolledtext.ScrolledText(
            parent,
            wrap="none",
            bg=self.editor_bg,
            fg="#cbd5e1",
            font=("Consolas", 10),
            relief="flat",
            padx=12,
            pady=12
        )
        self.output.pack(fill="both", expand=True, padx=18, pady=(0, 18))

    def set_sql(self, sql):
        self.editor.delete("1.0", "end")
        self.editor.insert("1.0", sql)

    def load_example(self):
        self.set_sql(
            "CREATE TABLE students (id INT, name TEXT, department TEXT, cgpa FLOAT);\n"
            "INSERT INTO students VALUES (1, 'Taiful', 'CSE', 3.50);\n"
            "INSERT INTO students VALUES (2, 'Rahim', 'CSE', 3.20);\n"
            "INSERT INTO students VALUES (3, 'Karim', 'EEE', 3.70);\n"
            "SELECT * FROM students;\n"
        )

    def refresh_tables(self):
        self.tables.delete(0, "end")

        if not os.path.exists(DATA_DIR):
            return

        for filename in sorted(os.listdir(DATA_DIR)):
            if filename.lower().endswith(".csv"):
                self.tables.insert("end", filename[:-4])

    def run_sql(self):
        if not os.path.exists(ENGINE):
            messagebox.showerror(
                "Engine not built",
                "Please run .\\build.bat first."
            )
            return

        sql = self.editor.get("1.0", "end").strip()

        if not sql:
            messagebox.showwarning(
                "Empty query",
                "Please enter an SQL query."
            )
            return

        started = datetime.now()

        try:
            result = subprocess.run(
                [ENGINE],
                input=sql,
                text=True,
                capture_output=True,
                cwd=BASE_DIR,
                timeout=15
            )

            elapsed = (datetime.now() - started).total_seconds()

            output = result.stdout

            if result.stderr:
                output += "\n" + result.stderr

            output += "\nExecution time: {:.4f} seconds".format(elapsed)

            self.output.delete("1.0", "end")
            self.output.insert("1.0", output)

            if result.returncode == 0:
                self.status.config(
                    text="● Engine Ready",
                    fg=self.green
                )
            else:
                self.status.config(
                    text="● Query Error",
                    fg=self.red
                )

            self.refresh_tables()

        except Exception as error:
            self.output.delete("1.0", "end")
            self.output.insert("1.0", "ERROR: " + str(error))

            self.status.config(
                text="● Error",
                fg=self.red
            )


if __name__ == "__main__":
    root = tk.Tk()
    app = MiniSQLEngineApp(root)
    root.mainloop()
