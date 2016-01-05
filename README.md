# c-gallery

Assignment: Digital Photo Album (Programming assignment 1 for COSC 58)

Write a program that allows a user to peruse a set of large images downloaded from a digital camera, and produce an html photo album.

That is:
A set of raw images, from the camera, lives in a directory.The program should take, as arguments, the names of a set of raw images. E.g., if photos/ is the directory and album is your program, one might invoke the program via the shell command line as album photos/*.jpg.

For each photo in this input set, the program should:
  Generate a thumbnail version (e.g., very small) of the photo (using the Linux command-line convert program)
  Display the thumbnail to the user (using the Linux command-line display program)
  Ask the user whether or not it should be rotated; if so, do it. (using the Linux command-line convert program)
  Ask the user for a caption, and collect it.
  Generate a properly oriented half-size version of the photo (using the Linux command-line convert program)
  When done, the program should leave the following in the directory in which it was invoked. (That is, if you were in foo/ when you invoked album, then all these things go into foo/)
    a file index.html containing, for each photo:
    a properly oriented thumbnail
    a captiona link from the thumbnail to a properly oriented medium-size version of the photo the thumbnails and medium-size versions.
