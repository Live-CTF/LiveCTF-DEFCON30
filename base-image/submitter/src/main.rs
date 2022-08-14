use serde::Deserialize;
use std::collections::HashMap;
use std::error::Error;
use std::process::Command;

#[derive(Debug, Deserialize)]
struct Config {
    apikey: String,
    challenge: String,
    url: String,
}

#[derive(Debug, Deserialize)]
#[serde(rename_all = "lowercase")]
#[serde(tag = "status")]
enum ApiResponse {
    Ok { winner: bool, message: String },
    Error { message: String },
}

fn read_config() -> std::io::Result<Config> {
    let content = std::fs::read_to_string("./.config.toml")?;
    Ok(toml::from_str(&content)?)
}

#[tokio::main]
async fn main() -> Result<(), Box<dyn Error>> {
    let output = Command::new("date")
                        .output()
                        .expect("Failed to execute command");
    unsafe { println!("{:?}", String::from_utf8_unchecked(output.stdout.clone())); }
    unsafe { eprintln!("{:?}", String::from_utf8_unchecked(output.stdout.clone())); }

    let mut args = std::env::args();

    let program = match args.next() {
        Some(x) => x,
        None => {
            println!("Error: argc == 0");
            return Ok(());
        }
    };

    let config = match read_config() {
        Ok(config) => config,
        Err(err) => {
            println!("Failed to read config: {}. Please contact an admin", err);
            return Ok(());
        }
    };

    let team_id = match args.next() {
        Some(x) => x,
        None => {
            println!("Usage: {} <team_id>", program);
            return Ok(());
        }
    };

    //println!("Team id: {}", team_id);
    //println!("Challenge id: {}", config.challenge);

    let flag_params = HashMap::from([
        ("team", team_id),
        ("challenge", config.challenge),
        ("apikey", config.apikey),
    ]);

    let client = reqwest::Client::new();
    let res = match client.post(config.url).json(&flag_params).send().await {
        Ok(res) => res,
        Err(err) => {
            println!(
                "Failed to contact flag server: {}. Please contact an admin.",
                err
            );
            return Ok(());
        }
    };

    let res_body = match res.text().await {
        Ok(body) => body,
        Err(err) => {
            println!(
                "Failed to get response from server: {}. Please contact an admin.",
                err
            );
            return Ok(());
        }
    };

    let res_data: ApiResponse = match serde_json::from_str(&res_body) {
        Ok(data) => data,
        Err(err) => {
            println!(
                "Failed to parse response from server: {}. Please contact an admin.",
                err
            );
            return Ok(());
        }
    };

    match res_data {
        ApiResponse::Error { message } => {
            println!("Error from server: {}. Please contact an admin.", message);
            return Ok(());
        }
        ApiResponse::Ok { winner, message } => {
            if winner {
                println!("You won! {}", message);
            } else {
                println!("You lost! {}", message);
            }
        }
    }

    Ok(())
}
