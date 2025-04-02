import tkinter as tk
from tkinter import ttk, messagebox
import serial
from serial.tools import list_ports
from utils import create_frame

class CommunicationGui(tk.Tk):
    def __init__(self):
        super().__init__()
        self.title("MCU Communication Tester")
        self.serial_conn = None  # Variable for the serial connection
        self.cmd_map = {"write":1, "read":2, "read many": 3}

        # --- Top Section ---
        top_frame = ttk.Frame(self)
        top_frame.pack(fill='x', padx=10, pady=10)
        
        # COM port selection
        ttk.Label(top_frame, text="COM:").grid(row=0, column=0, padx=5)
        self.com_combo = ttk.Combobox(top_frame, values=[])
        self.com_combo.grid(row=0, column=1, padx=5)
        
        # Refresh button (using unicode refresh symbol)
        self.refresh_button = ttk.Button(top_frame, text="⟳", command=self.refresh_ports)
        self.refresh_button.grid(row=0, column=2, padx=5)
        
        # Baud rate selection
        ttk.Label(top_frame, text="Baud:").grid(row=0, column=3, padx=5)
        self.baud_combo = ttk.Combobox(top_frame, values=["9600", "115200"])
        self.baud_combo.grid(row=0, column=4, padx=5)
        self.baud_combo.current(0)
        
        # Connect/Disconnect button
        self.connect_button = ttk.Button(top_frame, text="Connect", command=self.toggle_connection)
        self.connect_button.grid(row=0, column=5, padx=5)
        
        # --- Bottom Section ---
        lower_frame = ttk.Frame(self)
        lower_frame.pack(fill='x', padx=10, pady=10)
        
        # CMD selection
        ttk.Label(lower_frame, text="CMD:").grid(row=0, column=0, padx=5)
        self.cmd_combo = ttk.Combobox(lower_frame, values=list(self.cmd_map.keys()), state="disabled")
        self.cmd_combo.grid(row=0, column=1, padx=5)
        self.cmd_combo.current(0)
        
        # Channel count selection
        ttk.Label(lower_frame, text="Channel cnt:").grid(row=0, column=2, padx=5)
        self.chn_cnt_combo = ttk.Combobox(lower_frame, values=[str(i) for i in range(1, 11)], state="disabled")
        self.chn_cnt_combo.grid(row=0, column=3, padx=5)
        self.chn_cnt_combo.current(0)
        self.chn_cnt_combo.bind("<<ComboboxSelected>>", self.update_channels)
        
        # Frame for dynamically added channel widgets
        self.channels_frame = ttk.Frame(lower_frame)
        self.channels_frame.grid(row=1, column=0, columnspan=4, pady=10)
        self.channel_widgets = []  # List to store tuples (channel combobox, value entry)
        self.update_channels()  # Create default channel fields (1 channel)
        for cb, entry in self.channel_widgets:  # set disabled initial widgets
            cb.configure(state="disabled")
            entry.configure(state="disabled")
        
        # Send button
        self.send_button = ttk.Button(lower_frame, text="Send", command=self.send, state="disabled")
        self.send_button.grid(row=2, column=0, columnspan=4, pady=10)
        
        # Refresh COM ports on start
        self.refresh_ports()
    
    def refresh_ports(self):
        """Fetches the list of available COM ports and updates the combobox."""
        ports = [port.device for port in list_ports.comports()]
        self.com_combo['values'] = ports
        if ports:
            self.com_combo.current(0)
        else:
            self.com_combo.set("")
    
    def toggle_connection(self):
        """Connects or disconnects based on current connection state."""
        if self.serial_conn is None:
            self.connect()
        else:
            self.disconnect()
    
    def connect(self):
        """Attempts to establish the connection and enables the bottom panel."""
        com_port = self.com_combo.get()
        baud = self.baud_combo.get()
        try:
            self.serial_conn = serial.Serial(com_port, int(baud), timeout=0.1)
            self.set_lower_state("normal")
            self.connect_button.configure(text="Disconnect")
        except Exception as e:
            messagebox.showerror("Connection Error", str(e))
    
    def disconnect(self):
        """Closes the port, disables the bottom panel, and resets the button."""
        try:
            if self.serial_conn and self.serial_conn.is_open:
                self.serial_conn.close()
        except Exception as e:
            messagebox.showerror("Disconnection Error", str(e))
        finally:
            self.serial_conn = None
            self.set_lower_state("disabled")
            self.connect_button.configure(text="Connect")
    
    def set_lower_state(self, state: str):
        """Sets the state (disabled/normal) for the bottom panel widgets."""
        self.cmd_combo.configure(state=state)
        self.chn_cnt_combo.configure(state=state)
        self.send_button.configure(state=state)
        for cb, entry in self.channel_widgets:
            cb.configure(state=state)
            entry.configure(state=state)
    
    def update_channels(self, event=None):
        """Dynamically creates channel selection widgets based on the selected count."""
        # Remove previous widgets
        for widget in self.channels_frame.winfo_children():
            widget.destroy()
        self.channel_widgets = []
        
        try:
            count = int(self.chn_cnt_combo.get())
        except ValueError:
            count = 1

        # Add headers for the channel selection area
        header_channel = ttk.Label(self.channels_frame, text="Channel", font=('TkDefaultFont', 10, 'bold'))
        header_channel.grid(row=0, column=0, padx=5, pady=2)
        header_value = ttk.Label(self.channels_frame, text="Value", font=('TkDefaultFont', 10, 'bold'))
        header_value.grid(row=0, column=1, padx=5, pady=2)
        
        # Create widgets for each channel (starting from row 1)
        for i in range(count):
            #TODO: Change to maximum channels that can be obtainded on atmega
            # Combobox for channel number (1-10)
            cb = ttk.Combobox(self.channels_frame, values=[str(j) for j in range(1, 11)], width=3)
            cb.grid(row=i+1, column=0, padx=5, pady=2)
            cb.current(i if i < 10 else 0)  # default selection
            # Entry for value (0-100)
            entry = ttk.Entry(self.channels_frame, width=5)
            entry.grid(row=i+1, column=1, padx=5, pady=2)
            entry.insert(0, "0")
            self.channel_widgets.append((cb, entry))
    
    def send(self):
        """Sends data by retrieving and validating user inputs from the interface."""
        cmd = self.cmd_map[self.cmd_combo.get()]
        channels_data = []
        for _, (cb, entry) in enumerate(self.channel_widgets, start=1):
            channel_number = int(cb.get())
            try:
                value = int(entry.get())
                if not 0 <= value <= 100:
                    raise ValueError("Value out of range")
            except Exception as e:
                messagebox.showerror("Input Error", f"Channel {channel_number} error: {e}")
                return
            channels_data.append(channel_number)
            channels_data.append(value)

        frame_to_send = create_frame(cmd, channels_data)
        #clear input buffer
        self.serial_conn.reset_input_buffer()
        try:
            n = self.serial_conn.write(frame_to_send)
        except serial.SerialTimeoutException as e:
            messagebox.showerror("Sending Error", f"Timeout error")
        response = self.serial_conn.read_until('U', 25)
        print(n)
        if response:
            print(f"response: {response}")
        else:
            print("NO response")
        
        

if __name__ == "__main__":
    app = CommunicationGui()
    app.mainloop()