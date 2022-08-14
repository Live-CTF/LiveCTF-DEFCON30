#!/usr/bin/env python3

import argparse
from pathlib import Path
import subprocess
from typing import Optional

BASE_DIR = Path(__file__).parent
CHALLENGES_DIR = BASE_DIR / 'challenges'
IMAGE_PREFIX = 'livectf_'


parser = argparse.ArgumentParser()
subparsers = parser.add_subparsers(required=True, dest='command')

build_parser = subparsers.add_parser('build', help='Build a challenge image')
build_parser.add_argument('challenge', help='Challenge to build')

create_parser = subparsers.add_parser('create', help='Create a new challenge')
create_parser.add_argument('challenge', help='Challenge to create')

start_parser = subparsers.add_parser('start', help='Start a challenge')
start_parser.add_argument('challenge', help='Challenge to start')
start_parser.add_argument('port', default=8000, type=int, help='Challenge listen port')
start_parser.add_argument('team', type=int, default=None)

stop_parser = subparsers.add_parser('stop', help='Stop a challenge')
stop_parser.add_argument('challenge', help='Challenge to stop')
stop_parser.add_argument('team', type=int, default=None)

args = parser.parse_args()



def build_base_image():
    base_image_dir = BASE_DIR / 'base-image'
    print("Building base image...")
    subprocess.run(['make', '-C', base_image_dir], check=True)
    print("Base image up to date")


def build_challenge(name: str):
    build_base_image()

    chal_dir = CHALLENGES_DIR / name / 'challenge'

    chal_image = f'{IMAGE_PREFIX}{name}:latest'

    # Build challenge image
    print(f'Building challenge image for "{name}"...')
    subprocess.check_call(['docker', 'build', '-t', chal_image, chal_dir])

    # Copy handout file from challenge image
    print(f'Copying handout from "{chal_image}"...')
    container_id = subprocess.check_output(
        ['docker', 'create', chal_image],
    ).decode().strip()

    try:
        subprocess.check_call(['docker', 'cp', f'{container_id}:/handout.tar.gz', chal_dir])
    finally:
        subprocess.check_call(['docker', 'rm', '-f', container_id])

    print(f'Successfully built image and handout for "{name}"')


def create_challenge(name: str):
    chal_dir = CHALLENGES_DIR / name
    if chal_dir.exists():
        raise Exception('Challenge "{name}" already exists!')
    subprocess.check_call(['cp', '-r', 'template', chal_dir])

    print(f'Successfully initialized "{name}"')


def _check_if_challenge_needs_priv(chal_img):
    proc = subprocess.run(['docker', 'history', chal_img], check=True, stdout=subprocess.PIPE)
    return b'nsjail' in proc.stdout


def start_challenge(name: str, port: int, team: Optional[int]):
    chal_image = f'{IMAGE_PREFIX}{name}:latest'
    # Start challenge container
    print(f'Starting container for "{name}"...')
    # Special case non-privileged challenges
    run_privileged = _check_if_challenge_needs_priv(chal_image)

    run_args = [
        '--init',
        '-d',
        '-p', f'{port}:31337',
        '--name', f'{IMAGE_PREFIX}{name}' + f'_{team}' if team else '',
        chal_image
    ]

    if run_privileged:
        run_args.insert(0, '--privileged')

    subprocess.check_call(['docker', 'run'] + run_args)

    print(f'Successfully started "{name}"')


def stop_challenge(name: str, team: Optional[int]):
    # Stop and remove challenge container
    container_name = f'{IMAGE_PREFIX}{name}' + f'_{team}' if team else ''
    print(f'Stopping container for "{name}"...')
    subprocess.check_call(['docker', 'stop', container_name])
    print(f'Removing container for "{name}"...')
    subprocess.check_call(['docker', 'rm', container_name])

    print(f'Successfully stopped "{name}"')


# So we play nicely with tab complete
if args.challenge.endswith('/'):
    args.challenge = args.challenge[:-1]


if args.command == 'build':
    build_challenge(args.challenge)
elif args.command == 'create':
    create_challenge(args.challenge)
elif args.command == 'start':
    start_challenge(args.challenge, args.port, args.team)
elif args.command == 'stop':
    stop_challenge(args.challenge, args.team)
