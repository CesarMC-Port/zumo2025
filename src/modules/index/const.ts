export const config = {
  teamPhotoEnd: 460,
  splineEnd: 1400,
  photo2End: 2200,
  hidden: `
    opacity: 0;
    transform: scale(0) translateY(20px);
    transition: opacity 1s ease, transform 1s ease;
  `,
  visible: `
    opacity: 1;
    transform: scale(1) translateY(0);
    transition: opacity 1s ease, transform 1s ease;
  `
};