#!/usr/bin/env python3
"""
DeadlockDoctor Web GUI Server
Serves the web dashboard locally on http://localhost:5000
"""

import http.server
import socketserver
import os
import sys
import webbrowser

PORT = 5000
DIRECTORY = os.path.dirname(os.path.abspath(__file__))

class Handler(http.server.SimpleHTTPRequestHandler):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, directory=DIRECTORY, **kwargs)

def run_server():
    os.chdir(DIRECTORY)
    with socketserver.TCPServer(("", PORT), Handler) as httpd:
        url = f"http://localhost:{PORT}"
        print("=" * 60)
        print("   DEADLOCKDOCTOR WEB GUI VISUAL DASHBOARD SERVER")
        print("=" * 60)
        print(f" [*] Serving at: {url}")
        print(" [*] Press Ctrl+C to stop the server")
        print("=" * 60)
        
        # Optionally open in browser
        if "--no-browser" not in sys.argv:
            try:
                webbrowser.open(url)
            except Exception:
                pass
        
        try:
            httpd.serve_forever()
        except KeyboardInterrupt:
            print("\n[+] Server stopped.")

if __name__ == "__main__":
    run_server()
