package main

import (
	"database/sql"
	"encoding/json"
	"fmt"
	"net/http"
	"time"

	_ "github.com/mattn/go-sqlite3"
)

type temperatureReading struct {
	SensorID    string  `json:"sensor_id"`
	Temperature float64 `json:"temperature"`
	Timestamp   string  `json:"timestamp"`
}

func temperatureHandler(w http.ResponseWriter, r *http.Request) {
	if r.Method != http.MethodPost {
		http.Error(w, "Invalid request method", http.StatusMethodNotAllowed)
		return
	}

	decoder := json.NewDecoder(r.Body)
	var t temperatureReading
	err := decoder.Decode(&t)
	if err != nil {
		http.Error(w, "Error decoding request body", http.StatusBadRequest)
		return
	}

	// Open SQLite database
	db, err := sql.Open("sqlite3", "./temperatures.db")
	if err != nil {
		http.Error(w, "Error opening database", http.StatusInternalServerError)
		return
	}
	defer db.Close()

	// Parse timestamp
	print(t.Timestamp + " " + t.SensorID + " " + "\n")
	timestamp, err := time.Parse(time.RFC3339, t.Timestamp)
	if err != nil {
		http.Error(w, "Error parsing timestamp", http.StatusBadRequest)
		return
	}

	// Insert temperature reading into database
	_, err = db.Exec("INSERT INTO readings (sensor_id, temperature, timestamp) VALUES (?, ?, ?)", t.SensorID, t.Temperature, timestamp)
	if err != nil {
		http.Error(w, "Error inserting temperature reading into database", http.StatusInternalServerError)
		return
	}

	fmt.Printf("Stored temperature reading from sensor %s: %.2f at %s\n", t.SensorID, t.Temperature, t.Timestamp)
}

func main() {
	http.HandleFunc("/temperature", temperatureHandler)
	http.ListenAndServe(":8080", nil)
}
