# Contributing to ARK
Welcome to [report Issues](https://github.com/ArkGame/ARK/issues) or [pull requests](https://github.com/ArkGame/ARK/pulls). It's recommended to read the following Contributing Guide first before contributing. 

## Issues
We use Github Issues to track public bugs and feature requests.

### Search Known Issues First
Please search the existing issues to see if any similar issue or feature request has already been filed. You should make sure your issue isn't redundant.

### Reporting New Issues
If you open an issue, the more information the better. Such as detailed description, screenshot or video of your problem, logcat or code blocks for your crash.

## Pull Requests
We strongly welcome your pull request to make `ARK` better.

### Branch Management
There are two main branches here:

1. `master` branch.

	(1). It is the latest (pre-)release branch. We use `master` for tags.

	(2). **Don't submit any PR(Pull Request) on `master` branch.**
	
2. `develop` branch. 

	(1).There is a `develop` for `ARK` developing version. It is our stable developing branch. After full testing, `develop` branch will be merged to `master` branch for the next release.

	(2). **You are recommended to submit bugfix or feature PR on `develop` branch.**


Normal bugfix or feature request should be submitted to `develop` branch. After full testing, we will merge them to `master` branch for the next release. 


### Make Pull Requests
The code team will monitor all pull request, we run some code check and test on it. After all tests passed, we will accecpt this PR. But it won't merge to `master` branch at once, which have some delay.

Before submitting a pull request, please make sure the followings are done:

1. Fork the repo and create your branch from `master` or `develop`.
2. Update code or documentation if you have changed APIs or important functions.
3. Add the copyright notice to the top of any new files you've added.
4. Check your code lints and checkstyles.
5. Test and test again your code.
6. Now, you can submit your pull request on `develop` branch.

## Code Style Guide
Try to be similar with the other authers.

## License
By contributing to `ARK`, you agree that your contributions will be licensed
under [Apache LICENSE](https://github.com/ArkGame/ARK/blob/master/LICENSE)
