import { animate } from 'animejs';

export function initAnimatedLogo(element: HTMLElement) {
  animate(element, {
    // targets: element,
    scale: [0.8, 1],
    opacity: [0, 1],
    duration: 800,
    easing: "easeOutElastic(1, .6)"
  });

  const handleMouseEnter = () => {
    animate(element,{
    //   targets: element,
      rotate: [0, 360],
      scale: [1, 1.1, 1],
      duration: 600,
      easing: "easeInOutQuad",
    });
  };

  element.addEventListener("mouseenter", handleMouseEnter);
}