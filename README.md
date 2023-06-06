# Linux Device Driver

This project is a Linux device driver that provides encryption and decryption functionality for strings. The device driver can be loaded and run in Linux, allowing user/applications to interact with it by passing in strings to encrypt or decrypt.

#### I have made this project as a part of my Operating Systems class.

## Table of Contents

- [Introduction](#introduction)
- [Features](#features)
- [Requirements](#requirements)
- [Usage](#usage)
- [License](#license)

## Introduction

The Linux Device Driver project is aimed at developing a loadable kernel module (LKM) that provides encryption and decryption capabilities for strings. The device driver can be utilized by user applications to encrypt plain strings or decrypt encrypted strings using a specific algorithm. This README provides instructions on building, loading, and interacting with the device driver, along with examples and screenshots.

## Features

- Encryption of plain strings
- Decryption of encrypted strings
- Loadable kernel module (LKM) structure
- Open, release, read, write, and ioctl operations implemented
- Utilizes `printk` and `copy_to_user` functions
- Follows Linux file concept for read and write operations

## Requirements

To run this project, you need the following:

- Linux operating system (virtual machine recommended)
- GCC compiler installed
- Root privileges for building and loading the device driver

## Usage

To utilize the Linux Device Driver, refer to the writeup pdf document in this repository.

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for more information.
```
