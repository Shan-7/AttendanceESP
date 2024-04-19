import requests
import pandas as pd

# Firebase database URL
firebase_url = "https://present-2690e-default-rtdb.firebaseio.com/Login Page.json"

# Function to fetch data from Firebase and convert it to a DataFrame
def fetch_data_from_firebase(firebase_url):
    response = requests.get(firebase_url)
    data = response.json()
    df = pd.DataFrame.from_dict(data, orient='index')
    return df

# Function to save DataFrame to an Excel file
def save_to_excel(df, filename):
    df.to_excel(filename, index=False)

if __name__ == "__main__":
    # Fetch data from Firebase
    df = fetch_data_from_firebase(firebase_url)
    # Save data to Excel file
    save_to_excel(df, 'firebase_data.xlsx')
