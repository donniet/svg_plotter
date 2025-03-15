
#include "mesh.hpp"
#include "stroke_mesh.hpp"
#include "output/stl.hpp"
#include "output/obj.hpp"
#include "output/json_mesh.hpp"
#include "parser.hpp"
#include "plotter.hpp"
#include "fills.hpp"
#include "attribute_mesh.hpp"
#include "plot_cover.hpp"

#include <iostream>
#include <format>
#include <cmath>

#include "testing.hpp"

using std::sqrt;

int main(int ac, char * av[])
{
    // vector<Event> stroke;
    // stroke.insert(stroke.end(), {
    //     Event(Point(0,0), Vector(1,0), 0),
    //     Event(Point(1,0), Vector(1,0), 1)
    // });

    StrokeMesh mesher(10);

    // auto mesh = mesher(stroke);
    // std::cout << JSONMeshOutput(mesh.first);

    // SVGPath greenland(R"(M-11.608,540.851H21.74l76.801,10.737l21.221,18.54l10.105,19.526l31.027,20.497l8.469,13.093l17.239,10.76   c0,0,8.229,20.121,0,20.584s-194.169,2.924-194.169,2.924L-11.608,540.851z)");
    SVGPath greenland(R"(M 40.167969 302.42773 L 32.056641 350.23242 L 30.523438 350.23242 L 30.523438 353.51562 L 33.484375 353.51562 L 33.484375 378.45898 L 26.994141 378.45898 L 10.351562 392.40625 L -4.703125 378.39844 L -32.630859 404.38672 L -27.75 404.38672 L -27.75 427.44531 L -11.171875 427.44531 L -8.0351562 517.98438 L -47.753906 517.98438 L -47.753906 688.40234 L 656.99219 688.40234 L 656.99219 519.35156 L 709.93359 519.35156 L 716.29492 432.81641 C 716.29492 432.81641 684.47266 329.74055 684.47266 323.31055 C 684.47266 316.88055 650.12109 316.3457 650.12109 316.3457 L 618.30859 323.31055 L 615.76172 331.01367 L 606.85547 331.01367 L 600.49023 342.4668 L 587.76562 348.15625 L 580.13086 348.15625 L 563.58789 353.91992 L 550.86133 359.08594 L 540.17188 359.08594 L 530.5 359.18359 L 519.67578 359.08594 L 508.86719 371.34375 L 499.96094 371.34375 L 491.05078 383.1875 L 477.05469 383.1875 L 471.96289 397.18555 L 461.78125 397.18555 L 450.32812 410.04492 L 446.51172 418.81836 L 441.42383 446.74805 L 446.51172 458.26953 L 443.96875 473.22461 L 436.33008 496.44531 L 436.33008 517.98438 L 408.31641 517.98438 L 400.31641 503.98438 L 380.31641 511.98438 L 375.17383 517.98438 L 357.65625 517.98438 L 348.89648 511.1543 L 335.48047 511.32227 L 332.25195 517.98438 L 273.33203 517.98438 L 269.76758 509.24023 L 251.81836 507.86523 L 241.07227 507.23633 L 231.41992 512.32031 L 215.4668 511.09961 L 212.91602 517.98438 L 189.6543 517.98438 C 188.61656 512.94498 186.60156 508.00391 186.60156 508.00391 L 169.36328 497.24414 L 160.89453 484.15039 L 142.32031 471.88086 L 142.63477 471.88086 L 142.63477 457.51172 L 146.48438 457.51172 L 136.8457 446.7793 L 136.8457 443.59766 L 137.63086 443.59766 L 137.63086 438.11523 L 129.06445 438.11523 L 124.46289 432.99219 L 119.58008 438.42969 L 119.58008 422.08984 L 124.46289 422.08984 L 96.533203 396.10156 L 74.175781 416.90625 L 74.175781 395.91992 L 53.341797 378.45898 L 46.851562 378.45898 L 46.851562 353.51562 L 49.8125 353.51562 L 49.8125 350.23242 L 48.279297 350.23242 L 40.167969 302.42773 z M 561.04102 397.1875 L 572.49414 403.54883 L 578.85547 397.1875 L 601.76172 404.82227 L 601.76172 410.04688 L 613.21484 429.00391 L 624.66797 464.63281 L 624.66797 483.72266 L 624.66797 498.99219 L 613.98633 517.98438 L 607.10547 517.98438 L 613.76172 505.58008 L 612.15234 504.78711 C 612.15234 504.78711 603.78305 508.32445 592.74805 510.81445 C 581.71305 513.30245 571.06055 512.85547 571.06055 512.85547 L 569.64062 511.92188 C 569.64062 511.92188 569.38749 511.71793 569.09375 511.45898 A 9.5645208 2.6561444 72.9693 0 0 568.25195 502.08398 A 9.5645208 2.6561444 72.9693 0 0 562.91016 493.7168 A 9.5645208 2.6561444 72.9693 0 0 562.89844 493.72266 A 6.179266 9.2263966 87.26034 0 0 562.03906 493.20898 A 5.3092537 6.0922914 87.25461 0 0 562.3125 491.39844 A 5.3092537 6.0922914 87.25461 0 0 557.24023 486.45508 A 5.3092537 3.9171872 87.25461 0 0 557.60938 483.85547 A 5.3092537 3.9171872 87.25461 0 0 553.44141 478.74023 A 5.3092537 3.9171872 87.25461 0 0 549.7832 484.23047 A 5.3092537 3.9171872 87.25461 0 0 551.5293 488.375 A 5.3092537 6.0922914 87.25461 0 0 550.14258 491.98242 A 5.3092537 6.0922914 87.25461 0 0 550.46289 493.47266 A 6.179266 9.2263966 87.26034 0 0 548.80469 494.67383 A 2.6541359 9.5644903 29.98508 0 0 544.21289 499.80664 L 534.85156 494.00586 A 7.8329124 3.6469591 31.77642 0 0 529.86133 487.51367 A 7.8329124 3.6469591 31.77642 0 0 521.2832 486.48828 A 7.8329124 3.6469591 31.77642 0 0 526.02148 493.71484 A 7.8329124 3.6469591 31.77642 0 0 533.78906 495.43555 L 543.28125 501.31641 A 2.6541359 9.5644903 29.98508 0 0 543.26953 501.33789 A 2.6541359 9.5644903 29.98508 0 0 540.6875 507.13867 L 539.23242 506.88477 C 539.23242 506.88477 541.51795 504.92053 528.62695 507.39453 C 515.73695 509.86653 500.14258 502.13477 500.14258 502.13477 L 500.32422 505.93945 L 510.62891 517.31836 L 519.31836 517.98438 L 491.32422 517.98438 L 484.68945 508.90625 L 484.68945 478.63086 L 484.68945 458.26953 L 498.68555 446.74805 L 519.67383 426.45508 L 519.67383 410.04688 L 531.77344 410.04688 L 540.17188 404.82227 L 545.76953 410.04688 L 561.04102 406.0957 L 561.04102 397.1875 z M 547.79492 504.11328 L 548.20703 504.36914 A 7.7363329 7.754334 87.26034 0 0 548.1875 505.17969 A 7.7363329 7.754334 87.26034 0 0 549.20312 508.62109 L 545.19141 507.92188 A 2.6541359 9.5644903 29.98508 0 0 547.79492 504.11328 z M 563.67969 505.1543 A 9.5645208 2.6561444 72.9693 0 0 566.06055 510.12109 C 565.05447 510.55787 564.02344 511.10156 564.02344 511.10156 L 561.06055 510.60938 A 7.7363329 7.754334 87.26034 0 0 563.67969 505.1543 z )");
    // SVGPath greenland(R"(M 40.168 302.428 L 39.8434 304.34 L 39.5189 306.253 L 39.1945 308.165 L 38.8702 310.076 L 38.5458 311.988 L 38.2213 313.901 L 37.897 315.812 L 37.5725 317.725 L 37.2479 319.637 L 36.9234 321.55 L 36.5988 323.463 L 36.2743 325.375 L 35.95 327.287 L 35.6254 329.2 L 35.301 331.111 L 34.9767 333.023 L 34.6522 334.935 L 34.3279 336.847 L 34.0034 338.759 L 33.6789 340.672 L 33.3544 342.584 L 33.0299 344.496 L 32.7054 346.409 L 32.381 348.321 L 32.0566 350.232 L 30.5234 350.232 L 30.5234 351.874 L 30.5234 353.516 L 32.0039 353.516 L 33.4844 353.516 L 33.4844 355.434 L 33.4844 357.353 L 33.4844 359.271 L 33.4844 361.19 L 33.4844 363.108 L 33.4844 365.027 L 33.4844 366.946 L 33.4844 368.865 L 33.4844 370.784 L 33.4844 372.702 L 33.4844 374.622 L 33.4844 376.541 L 33.4844 378.459 L 31.8613 378.459 L 30.2385 378.459 L 28.6168 378.459 L 26.9941 378.459 L 25.4805 379.727 L 23.9675 380.995 L 22.4546 382.263 L 20.9418 383.531 L 19.429 384.799 L 17.9166 386.066 L 16.4044 387.334 L 14.8909 388.602 L 13.3781 389.87 L 11.865 391.138 L 10.3516 392.406 L 8.98233 391.132 L 7.6137 389.859 L 6.24514 388.585 L 4.87672 387.312 L 3.50825 386.039 L 2.14013 384.766 L 0.772166 383.493 L -0.596848 382.219 L -1.9654 380.946 L -3.33407 379.672 L -4.70312 378.398 L -6.10001 379.698 L -7.49591 380.997 L -8.89203 382.296 L -10.2889 383.596 L -11.6849 384.895 L -13.0819 386.195 L -14.4788 387.495 L -15.8752 388.795 L -17.2711 390.094 L -18.6669 391.392 L -20.0634 392.692 L -21.46 393.992 L -22.8566 395.291 L -24.2528 396.59 L -25.6487 397.889 L -27.0448 399.189 L -28.4409 400.488 L -29.8383 401.788 L -31.2341 403.087 L -32.6309 404.387 L -31.0039 404.387 L -29.3775 404.387 L -27.75 404.387 L -27.75 406.308 L -27.75 408.229 L -27.75 410.15 L -27.75 412.072 L -27.75 413.993 L -27.75 415.914 L -27.75 417.836 L -27.75 419.758 L -27.75 421.68 L -27.75 423.601 L -27.75 425.522 L -27.75 427.445 L -25.9085 427.445 L -24.066 427.445 L -22.2239 427.445 L -20.3813 427.445 L -18.5392 427.445 L -16.697 427.445 L -14.8562 427.445 L -13.0131 427.445 L -11.1719 427.445 L -11.1037 429.413 L -11.0355 431.381 L -10.9673 433.35 L -10.8991 435.318 L -10.8309 437.287 L -10.7628 439.254 L -10.6946 441.222 L -10.6264 443.19 L -10.5582 445.159 L -10.49 447.127 L -10.4218 449.095 L -10.3536 451.063 L -10.2855 453.031 L -10.2173 454.999 L -10.1491 456.967 L -10.0809 458.935 L -10.0127 460.904 L -9.94455 462.871 L -9.87639 464.838 L -9.80818 466.807 L -9.74001 468.775 L -9.67181 470.743 L -9.60364 472.711 L -9.53545 474.679 L -9.46727 476.648 L -9.39905 478.616 L -9.33083 480.586 L -9.26263 482.554 L -9.19442 484.523 L -9.12622 486.491 L -9.058 488.461 L -8.98981 490.429 L -8.92163 492.397 L -8.85342 494.366 L -8.78525 496.333 L -8.71709 498.301 L -8.6489 500.269 L -8.58072 502.237 L -8.51253 504.205 L -8.4443 506.175 L -8.3761 508.143 L -8.30789 510.112 L -8.23971 512.08 L -8.17155 514.047 L -8.10335 516.016 L -8.03516 517.984 L -10.0218 517.984 L -12.007 517.984 L -13.9926 517.984 L -15.9792 517.984 L -17.9646 517.984 L -19.9515 517.984 L -21.9382 517.984 L -23.924 517.984 L -25.9093 517.984 L -27.8944 517.984 L -29.8805 517.984 L -31.8668 517.984 L -33.8529 517.984 L -35.8387 517.984 L -37.8239 517.984 L -39.8094 517.984 L -41.795 517.984 L -43.7812 517.984 L -45.7668 517.984 L -47.7539 517.984 L -47.7539 519.966 L -47.7539 521.947 L -47.7539 523.929 L -47.7539 525.91 L -47.7539 527.892 L -47.7539 529.873 L -47.7539 531.855 L -47.7539 533.835 L -47.7539 535.817 L -47.7539 537.799 L -47.7539 539.781 L -47.7539 541.763 L -47.7539 543.745 L -47.7539 545.726 L -47.7539 547.707 L -47.7539 549.688 L -47.7539 551.67 L -47.7539 553.651 L -47.7539 555.634 L -47.7539 557.614 L -47.7539 559.595 L -47.7539 561.577 L -47.7539 563.559 L -47.7539 565.54 L -47.7539 567.522 L -47.7539 569.504 L -47.7539 571.486 L -47.7539 573.467 L -47.7539 575.449 L -47.7539 577.432 L -47.7539 579.413 L -47.7539 581.395 L -47.7539 583.377 L -47.7539 585.359 L -47.7539 587.341 L -47.7539 589.322 L -47.7539 591.305 L -47.7539 593.286 L -47.7539 595.266 L -47.7539 597.248 L -47.7539 599.229 L -47.7539 601.21 L -47.7539 603.193 L -47.7539 605.174 L -47.7539 607.156 L -47.7539 609.138 L -47.7539 611.12 L -47.7539 613.102 L -47.7539 615.084 L -47.7539 617.065 L -47.7539 619.046 L -47.7539 621.029 L -47.7539 623.011 L -47.7539 624.993 L -47.7539 626.975 L -47.7539 628.956 L -47.7539 630.939 L -47.7539 632.92 L -47.7539 634.902 L -47.7539 636.884 L -47.7539 638.867 L -47.7539 640.849 L -47.7539 642.831 L -47.7539 644.813 L -47.7539 646.794 L -47.7539 648.774 L -47.7539 650.755 L -47.7539 652.736 L -47.7539 654.717 L -47.7539 656.698 L -47.7539 658.68 L -47.7539 660.661 L -47.7539 662.642 L -47.7539 664.624 L -47.7539 666.605 L -47.7539 668.587 L -47.7539 670.569 L -47.7539 672.55 L -47.7539 674.532 L -47.7539 676.514 L -47.7539 678.495 L -47.7539 680.476 L -47.7539 682.457 L -47.7539 684.439 L -47.7539 686.422 L -47.7539 688.402 L -45.7575 688.402 L -43.7618 688.402 L -41.7658 688.402 L -39.7698 688.402 L -37.7725 688.402 L -35.7751 688.402 L -33.7795 688.402 L -31.784 688.402 L -29.7882 688.402 L -27.7926 688.402 L -25.7955 688.402 L -23.7989 688.402 L -21.8023 688.402 L -19.8064 688.402 L -17.8104 688.402 L -15.8138 688.402 L -13.8168 688.402 L -11.8207 688.402 L -9.82421 688.402 L -7.8275 688.402 L -5.83011 688.402 L -3.83302 688.402 L -1.83694 688.402 L 0.159476 688.402 L 2.15603 688.402 L 4.15273 688.402 L 6.14975 688.402 L 8.14552 688.402 L 10.1424 688.402 L 12.1392 688.402 L 14.1365 688.402 L 16.1332 688.402 L 18.1302 688.402 L 20.1265 688.402 L 22.1233 688.402 L 24.1196 688.402 L 26.1167 688.402 L 28.1138 688.402 L 30.1102 688.402 L 32.1072 688.402 L 34.1044 688.402 L 36.1009 688.402 L 38.0974 688.402 L 40.0933 688.402 L 42.089 688.402 L 44.0864 688.402 L 46.0827 688.402 L 48.0797 688.402 L 50.0769 688.402 L 52.0739 688.402 L 54.0704 688.402 L 56.066 688.402 L 58.0624 688.402 L 60.0595 688.402 L 62.0568 688.402 L 64.054 688.402 L 66.0509 688.402 L 68.047 688.402 L 70.0434 688.402 L 72.0396 688.402 L 74.0366 688.402 L 76.0329 688.402 L 78.0284 688.402 L 80.0239 688.402 L 82.0203 688.402 L 84.0175 688.402 L 86.014 688.402 L 88.0096 688.402 L 90.0065 688.402 L 92.0021 688.402 L 93.9986 688.402 L 95.9958 688.402 L 97.9922 688.402 L 99.9878 688.402 L 101.984 688.402 L 103.979 688.402 L 105.975 688.402 L 107.971 688.402 L 109.968 688.402 L 111.964 688.402 L 113.96 688.402 L 115.957 688.402 L 117.953 688.402 L 119.949 688.402 L 121.945 688.402 L 123.941 688.402 L 125.938 688.402 L 127.934 688.402 L 129.93 688.402 L 131.926 688.402 L 133.922 688.402 L 135.918 688.402 L 137.914 688.402 L 139.91 688.402 L 141.906 688.402 L 143.902 688.402 L 145.899 688.402 L 147.896 688.402 L 149.892 688.402 L 151.889 688.402 L 153.885 688.402 L 155.882 688.402 L 157.878 688.402 L 159.874 688.402 L 161.87 688.402 L 163.867 688.402 L 165.864 688.402 L 167.86 688.402 L 169.857 688.402 L 171.854 688.402 L 173.85 688.402 L 175.846 688.402 L 177.842 688.402 L 179.837 688.402 L 181.833 688.402 L 183.829 688.402 L 185.824 688.402 L 187.821 688.402 L 189.818 688.402 L 191.815 688.402 L 193.811 688.402 L 195.807 688.402 L 197.804 688.402 L 199.8 688.402 L 201.797 688.402 L 203.793 688.402 L 205.789 688.402 L 207.786 688.402 L 209.781 688.402 L 211.778 688.402 L 213.775 688.402 L 215.771 688.402 L 217.767 688.402 L 219.764 688.402 L 221.761 688.402 L 223.756 688.402 L 225.752 688.402 L 227.748 688.402 L 229.744 688.402 L 231.741 688.402 L 233.737 688.402 L 235.733 688.402 L 237.729 688.402 L 239.724 688.402 L 241.722 688.402 L 243.719 688.402 L 245.715 688.402 L 247.71 688.402 L 249.707 688.402 L 251.704 688.402 L 253.701 688.402 L 255.697 688.402 L 257.694 688.402 L 259.69 688.402 L 261.687 688.402 L 263.684 688.402 L 265.68 688.402 L 267.677 688.402 L 269.674 688.402 L 271.671 688.402 L 273.667 688.402 L 275.663 688.402 L 277.658 688.402 L 279.654 688.402 L 281.65 688.402 L 283.647 688.402 L 285.644 688.402 L 287.641 688.402 L 289.637 688.402 L 291.634 688.402 L 293.631 688.402 L 295.627 688.402 L 297.623 688.402 L 299.619 688.402 L 301.616 688.402 L 303.613 688.402 L 305.609 688.402 L 307.605 688.402 L 309.602 688.402 L 311.599 688.402 L 313.596 688.402 L 315.593 688.402 L 317.59 688.402 L 319.587 688.402 L 321.584 688.402 L 323.58 688.402 L 325.575 688.402 L 327.572 688.402 L 329.568 688.402 L 331.565 688.402 L 333.561 688.402 L 335.557 688.402 L 337.555 688.402 L 339.55 688.402 L 341.546 688.402 L 343.542 688.402 L 345.538 688.402 L 347.534 688.402 L 349.53 688.402 L 351.527 688.402 L 353.524 688.402 L 355.52 688.402 L 357.517 688.402 L 359.512 688.402 L 361.509 688.402 L 363.506 688.402 L 365.503 688.402 L 367.501 688.402 L 369.498 688.402 L 371.495 688.402 L 373.492 688.402 L 375.489 688.402 L 377.485 688.402 L 379.481 688.402 L 381.478 688.402 L 383.475 688.402 L 385.471 688.402 L 387.468 688.402 L 389.464 688.402 L 391.461 688.402 L 393.456 688.402 L 395.453 688.402 L 397.449 688.402 L 399.446 688.402 L 401.443 688.402 L 403.44 688.402 L 405.437 688.402 L 407.434 688.402 L 409.431 688.402 L 411.426 688.402 L 413.422 688.402 L 415.419 688.402 L 417.416 688.402 L 419.413 688.402 L 421.409 688.402 L 423.405 688.402 L 425.401 688.402 L 427.398 688.402 L 429.395 688.402 L 431.392 688.402 L 433.389 688.402 L 435.384 688.402 L 437.381 688.402 L 439.377 688.402 L 441.373 688.402 L 443.369 688.402 L 445.366 688.402 L 447.363 688.402 L 449.36 688.402 L 451.356 688.402 L 453.353 688.402 L 455.349 688.402 L 457.345 688.402 L 459.343 688.402 L 461.338 688.402 L 463.334 688.402 L 465.33 688.402 L 467.326 688.402 L 469.323 688.402 L 471.318 688.402 L 473.315 688.402 L 475.311 688.402 L 477.308 688.402 L 479.304 688.402 L 481.301 688.402 L 483.297 688.402 L 485.294 688.402 L 487.29 688.402 L 489.286 688.402 L 491.284 688.402 L 493.279 688.402 L 495.275 688.402 L 497.271 688.402 L 499.268 688.402 L 501.265 688.402 L 503.262 688.402 L 505.259 688.402 L 507.255 688.402 L 509.251 688.402 L 511.247 688.402 L 513.243 688.402 L 515.24 688.402 L 517.237 688.402 L 519.234 688.402 L 521.231 688.402 L 523.227 688.402 L 525.223 688.402 L 527.219 688.402 L 529.215 688.402 L 531.211 688.402 L 533.207 688.402 L 535.204 688.402 L 537.201 688.402 L 539.197 688.402 L 541.193 688.402 L 543.189 688.402 L 545.186 688.402 L 547.182 688.402 L 549.178 688.402 L 551.175 688.402 L 553.172 688.402 L 555.169 688.402 L 557.165 688.402 L 559.162 688.402 L 561.159 688.402 L 563.155 688.402 L 565.15 688.402 L 567.147 688.402 L 569.143 688.402 L 571.141 688.402 L 573.138 688.402 L 575.134 688.402 L 577.13 688.402 L 579.126 688.402 L 581.123 688.402 L 583.12 688.402 L 585.116 688.402 L 587.113 688.402 L 589.11 688.402 L 591.106 688.402 L 593.103 688.402 L 595.099 688.402 L 597.096 688.402 L 599.093 688.402 L 601.09 688.402 L 603.087 688.402 L 605.083 688.402 L 607.079 688.402 L 609.076 688.402 L 611.072 688.402 L 613.069 688.402 L 615.065 688.402 L 617.061 688.402 L 619.057 688.402 L 621.053 688.402 L 623.049 688.402 L 625.047 688.402 L 627.043 688.402 L 629.041 688.402 L 631.037 688.402 L 633.034 688.402 L 635.03 688.402 L 637.027 688.402 L 639.023 688.402 L 641.019 688.402 L 643.015 688.402 L 645.013 688.402 L 647.01 688.402 L 649.006 688.402 L 651.001 688.402 L 652.998 688.402 L 654.994 688.402 L 656.992 688.402 L 656.992 686.414 L 656.992 684.425 L 656.992 682.435 L 656.992 680.446 L 656.992 678.457 L 656.992 676.469 L 656.992 674.481 L 656.992 672.492 L 656.992 670.503 L 656.992 668.514 L 656.992 666.525 L 656.992 664.537 L 656.992 662.549 L 656.992 660.56 L 656.992 658.571 L 656.992 656.582 L 656.992 654.593 L 656.992 652.604 L 656.992 650.615 L 656.992 648.626 L 656.992 646.637 L 656.992 644.647 L 656.992 642.658 L 656.992 640.668 L 656.992 638.68 L 656.992 636.691 L 656.992 634.702 L 656.992 632.713 L 656.992 630.724 L 656.992 628.735 L 656.992 626.746 L 656.992 624.757 L 656.992 622.768 L 656.992 620.78 L 656.992 618.791 L 656.992 616.802 L 656.992 614.813 L 656.992 612.824 L 656.992 610.836 L 656.992 608.846 L 656.992 606.858 L 656.992 604.868 L 656.992 602.879 L 656.992 600.891 L 656.992 598.902 L 656.992 596.914 L 656.992 594.926 L 656.992 592.937 L 656.992 590.947 L 656.992 588.959 L 656.992 586.97 L 656.992 584.981 L 656.992 582.992 L 656.992 581.002 L 656.992 579.013 L 656.992 577.025 L 656.992 575.037 L 656.992 573.048 L 656.992 571.059 L 656.992 569.07 L 656.992 567.081 L 656.992 565.092 L 656.992 563.103 L 656.992 561.114 L 656.992 559.124 L 656.992 557.136 L 656.992 555.147 L 656.992 553.158 L 656.992 551.171 L 656.992 549.182 L 656.992 547.194 L 656.992 545.206 L 656.992 543.217 L 656.992 541.227 L 656.992 539.239 L 656.992 537.25 L 656.992 535.261 L 656.992 533.273 L 656.992 531.285 L 656.992 529.297 L 656.992 527.307 L 656.992 525.319 L 656.992 523.33 L 656.992 521.34 L 656.992 519.352 L 658.953 519.352 L 660.914 519.352 L 662.875 519.352 L 664.835 519.352 L 666.796 519.352 L 668.757 519.352 L 670.719 519.352 L 672.68 519.352 L 674.641 519.352 L 676.602 519.352 L 678.563 519.352 L 680.524 519.352 L 682.484 519.352 L 684.445 519.352 L 686.407 519.352 L 688.368 519.352 L 690.328 519.352 L 692.288 519.352 L 694.249 519.352 L 696.209 519.352 L 698.17 519.352 L 700.131 519.352 L 702.091 519.352 L 704.053 519.352 L 706.013 519.352 L 707.974 519.352 L 709.934 519.352 L 710.078 517.384 L 710.223 515.417 L 710.367 513.45 L 710.512 511.483 L 710.657 509.516 L 710.801 507.549 L 710.946 505.582 L 711.09 503.616 L 711.235 501.65 L 711.379 499.683 L 711.524 497.716 L 711.669 495.75 L 711.813 493.783 L 711.958 491.816 L 712.102 489.85 L 712.247 487.883 L 712.391 485.917 L 712.536 483.95 L 712.681 481.984 L 712.825 480.018 L 712.97 478.052 L 713.114 476.085 L 713.259 474.119 L 713.403 472.153 L 713.548 470.187 L 713.692 468.22 L 713.837 466.254 L 713.981 464.288 L 714.126 462.32 L 714.271 460.352 L 714.415 458.386 L 714.56 456.419 L 714.704 454.452 L 714.849 452.485 L 714.994 450.518 L 715.138 448.552 L 715.283 446.585 L 715.427 444.617 L 715.572 442.65 L 715.717 440.683 L 715.861 438.716 L 716.006 436.75 L 716.15 434.783 L 716.295 432.816 L 715.715 430.936 L 715.137 429.057 L 714.56 427.177 L 713.983 425.298 L 713.406 423.416 L 712.829 421.536 L 712.254 419.655 L 711.679 417.775 L 711.104 415.894 L 710.53 414.014 L 709.956 412.133 L 709.383 410.251 L 708.81 408.37 L 708.237 406.489 L 707.665 404.607 L 707.094 402.725 L 706.523 400.843 L 705.953 398.961 L 705.383 397.079 L 704.814 395.196 L 704.245 393.313 L 703.676 391.43 L 703.109 389.547 L 702.542 387.664 L 701.975 385.78 L 701.41 383.897 L 700.845 382.013 L 700.281 380.13 L 699.717 378.246 L 699.155 376.362 L 698.592 374.477 L 698.031 372.592 L 697.471 370.707 L 696.912 368.822 L 696.354 366.937 L 695.797 365.051 L 695.241 363.165 L 694.686 361.277 L 694.133 359.39 L 693.58 357.502 L 693.03 355.613 L 692.481 353.724 L 691.933 351.835 L 691.388 349.945 L 690.845 348.055 L 690.305 346.164 L 689.767 344.271 L 689.232 342.378 L 688.701 340.484 L 688.174 338.589 L 687.652 336.692 L 687.136 334.793 L 686.628 332.895 L 686.129 330.992 L 685.645 329.086 L 685.182 327.174 L 684.758 325.253 L 684.473 323.311 L 683.634 321.663 L 682.048 320.621 L 680.29 319.891 L 678.471 319.329 L 676.623 318.872 L 674.758 318.489 L 672.883 318.161 L 671.001 317.877 L 669.112 317.628 L 667.221 317.409 L 665.326 317.215 L 663.43 317.044 L 661.532 316.894 L 659.632 316.761 L 657.731 316.646 L 655.83 316.546 L 653.928 316.462 L 652.026 316.394 L 650.121 316.346 L 648.25 316.755 L 646.379 317.165 L 644.508 317.575 L 642.637 317.984 L 640.766 318.394 L 638.896 318.803 L 637.025 319.213 L 635.153 319.623 L 633.282 320.032 L 631.411 320.442 L 629.539 320.852 L 627.667 321.262 L 625.795 321.671 L 623.923 322.081 L 622.052 322.491 L 620.18 322.901 L 618.309 323.311 L 617.799 324.851 L 617.29 326.392 L 616.78 327.933 L 616.271 329.474 L 615.762 331.014 L 613.981 331.014 L 612.199 331.014 L 610.417 331.014 L 608.636 331.014 L 606.855 331.014 L 605.946 332.65 L 605.037 334.286 L 604.127 335.923 L 603.218 337.558 L 602.309 339.195 L 601.4 340.83 L 600.49 342.467 L 598.672 343.28 L 596.855 344.092 L 595.036 344.905 L 593.219 345.718 L 591.401 346.531 L 589.583 347.344 L 587.766 348.156 L 585.856 348.156 L 583.947 348.156 L 582.04 348.156 L 580.131 348.156 L 578.293 348.796 L 576.455 349.437 L 574.617 350.077 L 572.778 350.718 L 570.94 351.359 L 569.101 351.999 L 567.264 352.639 L 565.426 353.279 L 563.588 353.92 L 561.77 354.658 L 559.952 355.396 L 558.133 356.134 L 556.316 356.872 L 554.497 357.61 L 552.679 358.348 L 550.861 359.086 L 549.08 359.086 L 547.297 359.086 L 545.516 359.086 L 543.735 359.086 L 541.953 359.086 L 540.172 359.086 L 538.238 359.105 L 536.303 359.125 L 534.368 359.145 L 532.433 359.164 L 530.5 359.184 L 528.696 359.167 L 526.891 359.151 L 525.088 359.135 L 523.284 359.118 L 521.48 359.102 L 519.676 359.086 L 518.475 360.448 L 517.274 361.81 L 516.073 363.172 L 514.872 364.534 L 513.671 365.896 L 512.469 367.258 L 511.269 368.62 L 510.068 369.982 L 508.867 371.344 L 507.086 371.344 L 505.304 371.344 L 503.523 371.344 L 501.741 371.344 L 499.961 371.344 L 498.847 372.824 L 497.734 374.304 L 496.62 375.785 L 495.506 377.266 L 494.392 378.746 L 493.278 380.227 L 492.164 381.708 L 491.051 383.188 L 489.051 383.188 L 487.052 383.188 L 485.052 383.188 L 483.053 383.188 L 481.053 383.188 L 479.054 383.188 L 477.055 383.188 L 476.418 384.937 L 475.782 386.687 L 475.145 388.437 L 474.509 390.187 L 473.872 391.936 L 473.236 393.687 L 472.599 395.437 L 471.963 397.186 L 470.266 397.186 L 468.568 397.186 L 466.872 397.186 L 465.175 397.186 L 463.478 397.186 L 461.781 397.186 L 460.509 398.614 L 459.236 400.043 L 457.964 401.472 L 456.691 402.901 L 455.418 404.33 L 454.145 405.759 L 452.873 407.187 L 451.601 408.616 L 450.328 410.045 L 449.565 411.799 L 448.801 413.555 L 448.038 415.309 L 447.275 417.065 L 446.512 418.818 L 446.172 420.681 L 445.833 422.543 L 445.494 424.405 L 445.155 426.268 L 444.815 428.13 L 444.476 429.991 L 444.137 431.852 L 443.798 433.714 L 443.459 435.577 L 443.12 437.439 L 442.781 439.3 L 442.441 441.162 L 442.102 443.024 L 441.763 444.886 L 441.424 446.748 L 442.151 448.394 L 442.878 450.04 L 443.605 451.686 L 444.331 453.331 L 445.058 454.978 L 445.785 456.623 L 446.512 458.27 L 446.194 460.139 L 445.876 462.008 L 445.558 463.878 L 445.24 465.748 L 444.922 467.616 L 444.604 469.486 L 444.286 471.356 L 443.969 473.225 L 443.381 475.011 L 442.794 476.797 L 442.206 478.583 L 441.619 480.369 L 441.031 482.155 L 440.443 483.941 L 439.856 485.728 L 439.268 487.514 L 438.68 489.301 L 438.093 491.086 L 437.505 492.873 L 436.918 494.66 L 436.33 496.445 L 436.33 498.404 L 436.33 500.362 L 436.33 502.32 L 436.33 504.278 L 436.33 506.236 L 436.33 508.194 L 436.33 510.151 L 436.33 512.109 L 436.33 514.067 L 436.33 516.026 L 436.33 517.984 L 434.462 517.984 L 432.594 517.984 L 430.726 517.984 L 428.858 517.984 L 426.991 517.984 L 425.124 517.984 L 423.257 517.984 L 421.388 517.984 L 419.52 517.984 L 417.652 517.984 L 415.786 517.984 L 413.919 517.984 L 412.051 517.984 L 410.184 517.984 L 408.316 517.984 L 407.428 516.429 L 406.539 514.873 L 405.65 513.318 L 404.761 511.762 L 403.872 510.206 L 402.983 508.65 L 402.094 507.096 L 401.205 505.539 L 400.316 503.984 L 398.497 504.712 L 396.679 505.439 L 394.861 506.167 L 393.043 506.894 L 391.225 507.621 L 389.408 508.348 L 387.59 509.075 L 385.772 509.802 L 383.953 510.53 L 382.135 511.257 L 380.316 511.984 L 379.03 513.485 L 377.745 514.985 L 376.46 516.484 L 375.174 517.984 L 373.228 517.984 L 371.281 517.984 L 369.335 517.984 L 367.388 517.984 L 365.441 517.984 L 363.495 517.984 L 361.549 517.984 L 359.603 517.984 L 357.656 517.984 L 356.196 516.846 L 354.737 515.708 L 353.277 514.57 L 351.817 513.431 L 350.357 512.293 L 348.896 511.154 L 346.98 511.178 L 345.063 511.202 L 343.146 511.226 L 341.231 511.25 L 339.313 511.274 L 337.398 511.298 L 335.48 511.322 L 334.673 512.988 L 333.866 514.654 L 333.059 516.319 L 332.252 517.984 L 330.287 517.984 L 328.323 517.984 L 326.36 517.984 L 324.396 517.984 L 322.432 517.984 L 320.468 517.984 L 318.504 517.984 L 316.539 517.984 L 314.575 517.984 L 312.611 517.984 L 310.647 517.984 L 308.683 517.984 L 306.72 517.984 L 304.755 517.984 L 302.791 517.984 L 300.827 517.984 L 298.863 517.984 L 296.898 517.984 L 294.935 517.984 L 292.971 517.984 L 291.007 517.984 L 289.043 517.984 L 287.08 517.984 L 285.115 517.984 L 283.15 517.984 L 281.187 517.984 L 279.224 517.984 L 277.259 517.984 L 275.296 517.984 L 273.332 517.984 L 272.619 516.236 L 271.906 514.486 L 271.193 512.737 L 270.48 510.988 L 269.768 509.24 L 267.973 509.103 L 266.179 508.965 L 264.383 508.828 L 262.589 508.69 L 260.794 508.553 L 258.998 508.415 L 257.204 508.278 L 255.409 508.14 L 253.614 508.003 L 251.818 507.865 L 250.027 507.76 L 248.235 507.656 L 246.445 507.551 L 244.654 507.446 L 242.863 507.341 L 241.072 507.236 L 239.463 508.084 L 237.854 508.931 L 236.246 509.778 L 234.637 510.626 L 233.028 511.473 L 231.42 512.32 L 229.426 512.168 L 227.432 512.015 L 225.437 511.863 L 223.443 511.71 L 221.449 511.557 L 219.455 511.405 L 217.46 511.252 L 215.467 511.1 L 214.829 512.821 L 214.191 514.543 L 213.554 516.263 L 212.916 517.984 L 210.978 517.984 L 209.04 517.984 L 207.102 517.984 L 205.163 517.984 L 203.225 517.984 L 201.287 517.984 L 199.348 517.984 L 197.409 517.984 L 195.471 517.984 L 193.533 517.984 L 191.594 517.984 L 189.654 517.984 L 189.269 516.286 L 188.825 514.602 L 188.332 512.931 L 187.797 511.275 L 187.221 509.631 L 186.602 508.004 L 185.034 507.025 L 183.467 506.047 L 181.9 505.069 L 180.333 504.091 L 178.765 503.112 L 177.198 502.135 L 175.632 501.157 L 174.065 500.179 L 172.498 499.201 L 170.931 498.222 L 169.363 497.244 L 168.305 495.607 L 167.246 493.971 L 166.187 492.334 L 165.129 490.697 L 164.07 489.061 L 163.011 487.423 L 161.953 485.786 L 160.895 484.15 L 159.347 483.128 L 157.799 482.106 L 156.252 481.084 L 154.704 480.061 L 153.156 479.039 L 151.609 478.017 L 150.061 476.994 L 148.512 475.971 L 146.965 474.949 L 145.417 473.927 L 143.869 472.904 L 142.32 471.881 L 142.635 471.881 L 142.635 470.085 L 142.635 468.289 L 142.635 466.492 L 142.635 464.696 L 142.635 462.9 L 142.635 461.104 L 142.635 459.307 L 142.635 457.512 L 144.56 457.512 L 146.484 457.512 L 145.279 456.17 L 144.075 454.829 L 142.87 453.487 L 141.665 452.145 L 140.46 450.804 L 139.255 449.462 L 138.05 448.12 L 136.846 446.779 L 136.846 445.188 L 136.846 443.598 L 137.631 443.598 L 137.631 441.77 L 137.631 439.943 L 137.631 438.115 L 135.918 438.115 L 134.204 438.115 L 132.491 438.115 L 130.777 438.115 L 129.064 438.115 L 127.914 436.834 L 126.763 435.553 L 125.613 434.273 L 124.463 432.992 L 123.242 434.352 L 122.021 435.712 L 120.801 437.07 L 119.58 438.43 L 119.58 436.615 L 119.58 434.799 L 119.58 432.983 L 119.58 431.167 L 119.58 429.351 L 119.58 427.536 L 119.58 425.721 L 119.58 423.905 L 119.58 422.09 L 121.208 422.09 L 122.835 422.09 L 124.463 422.09 L 123.066 420.79 L 121.67 419.491 L 120.274 418.192 L 118.877 416.892 L 117.481 415.593 L 116.083 414.293 L 114.686 412.993 L 113.29 411.694 L 111.894 410.395 L 110.498 409.096 L 109.102 407.796 L 107.705 406.497 L 106.308 405.197 L 104.912 403.898 L 103.516 402.599 L 102.12 401.3 L 100.723 400.001 L 99.326 398.7 L 97.9301 397.401 L 96.5332 396.102 L 95.1357 397.402 L 93.7377 398.703 L 92.3409 400.003 L 90.9429 401.304 L 89.545 402.604 L 88.1481 403.904 L 86.7511 405.204 L 85.3538 406.505 L 83.957 407.804 L 82.5603 409.104 L 81.1628 410.404 L 79.7647 411.706 L 78.368 413.005 L 76.9712 414.305 L 75.5741 415.605 L 74.1758 416.906 L 74.1758 414.998 L 74.1758 413.09 L 74.1758 411.182 L 74.1758 409.274 L 74.1758 407.367 L 74.1758 405.459 L 74.1758 403.553 L 74.1758 401.644 L 74.1758 399.736 L 74.1758 397.828 L 74.1758 395.92 L 72.6879 394.673 L 71.1995 393.426 L 69.7112 392.178 L 68.2225 390.93 L 66.7342 389.683 L 65.246 388.436 L 63.7587 387.189 L 62.2707 385.942 L 60.7819 384.695 L 59.2946 383.448 L 57.8059 382.2 L 56.318 380.953 L 54.8294 379.706 L 53.3418 378.459 L 51.7187 378.459 L 50.096 378.459 L 48.4743 378.459 L 46.8516 378.459 L 46.8516 376.541 L 46.8516 374.622 L 46.8516 372.704 L 46.8516 370.785 L 46.8516 368.866 L 46.8516 366.947 L 46.8516 365.028 L 46.8516 363.11 L 46.8516 361.19 L 46.8516 359.272 L 46.8516 357.353 L 46.8516 355.434 L 46.8516 353.516 L 48.332 353.516 L 49.8125 353.516 L 49.8125 351.874 L 49.8125 350.232 L 48.2793 350.232 L 47.9547 348.32 L 47.6303 346.407 L 47.3059 344.495 L 46.9815 342.584 L 46.6571 340.672 L 46.3326 338.759 L 46.0083 336.848 L 45.6838 334.936 L 45.3592 333.023 L 45.0347 331.11 L 44.7101 329.197 L 44.3856 327.285 L 44.0613 325.373 L 43.7367 323.46 L 43.4123 321.549 L 43.0881 319.637 L 42.7636 317.725 L 42.4392 315.813 L 42.1148 313.901 L 41.7902 311.989 L 41.4657 310.076 L 41.1413 308.164 L 40.8167 306.251 L 40.4924 304.34 L 40.168 302.428 M 561.041 397.188 L 562.677 398.096 L 564.313 399.005 L 565.95 399.914 L 567.585 400.822 L 569.222 401.731 L 570.857 402.64 L 572.494 403.549 L 573.766 402.277 L 575.039 401.004 L 576.311 399.732 L 577.584 398.459 L 578.855 397.188 L 580.617 397.775 L 582.379 398.362 L 584.141 398.949 L 585.903 399.536 L 587.665 400.124 L 589.427 400.711 L 591.189 401.298 L 592.951 401.886 L 594.714 402.473 L 596.475 403.06 L 598.238 403.648 L 600 404.235 L 601.762 404.822 L 601.762 406.564 L 601.762 408.305 L 601.762 410.047 L 602.716 411.626 L 603.67 413.206 L 604.624 414.785 L 605.579 416.365 L 606.533 417.945 L 607.487 419.524 L 608.442 421.104 L 609.397 422.684 L 610.351 424.264 L 611.305 425.843 L 612.26 427.423 L 613.215 429.004 L 613.817 430.879 L 614.42 432.754 L 615.023 434.63 L 615.626 436.505 L 616.229 438.381 L 616.832 440.257 L 617.435 442.133 L 618.038 444.008 L 618.64 445.882 L 619.243 447.757 L 619.846 449.632 L 620.448 451.506 L 621.051 453.382 L 621.654 455.258 L 622.257 457.132 L 622.859 459.007 L 623.462 460.883 L 624.065 462.757 L 624.668 464.633 L 624.668 466.542 L 624.668 468.451 L 624.668 470.361 L 624.668 472.269 L 624.668 474.178 L 624.668 476.087 L 624.668 477.995 L 624.668 479.904 L 624.668 481.813 L 624.668 483.723 L 624.668 485.631 L 624.668 487.54 L 624.668 489.449 L 624.668 491.358 L 624.668 493.266 L 624.668 495.175 L 624.668 497.084 L 624.668 498.992 L 623.696 500.72 L 622.725 502.446 L 621.754 504.173 L 620.783 505.899 L 619.812 507.625 L 618.842 509.351 L 617.871 511.077 L 616.9 512.804 L 615.929 514.531 L 614.958 516.257 L 613.986 517.984 L 612.266 517.984 L 610.545 517.984 L 608.826 517.984 L 607.105 517.984 L 607.938 516.434 L 608.769 514.884 L 609.602 513.333 L 610.434 511.782 L 611.266 510.232 L 612.098 508.681 L 612.93 507.13 L 613.762 505.58 L 612.152 504.787 L 610.434 505.471 L 608.703 506.118 L 606.961 506.738 L 605.211 507.333 L 603.453 507.904 L 601.686 508.451 L 599.911 508.974 L 598.131 509.472 L 596.344 509.946 L 594.549 510.393 L 592.748 510.814 L 590.806 511.224 L 588.854 511.582 L 586.893 511.891 L 584.925 512.156 L 582.953 512.378 L 580.976 512.559 L 578.995 512.7 L 577.013 512.802 L 575.03 512.865 L 573.045 512.887 L 571.061 512.855 L 569.641 511.922 L 569.094 511.459 L 569.483 509.601 L 569.421 507.692 L 569.159 505.798 L 568.761 503.927 L 568.252 502.084 L 567.712 500.465 L 567.087 498.877 L 566.368 497.329 L 565.527 495.844 L 564.446 494.437 L 562.91 493.717 L 562.898 493.723 L 562.039 493.209 L 562.313 491.399 L 561.83 489.553 L 560.68 488.028 L 559.082 486.979 L 557.24 486.455 L 557.551 485.174 L 557.609 483.856 L 557.236 481.903 L 556.283 480.16 L 555.029 479.108 L 553.441 478.74 L 551.755 479.356 L 550.568 480.72 L 549.928 482.42 L 549.783 484.231 L 550.021 485.744 L 550.595 487.164 L 551.529 488.375 L 550.469 490.039 L 550.143 491.983 L 550.463 493.473 L 549.599 494.023 L 548.805 494.674 L 547.451 495.751 L 546.269 497.018 L 545.197 498.38 L 544.213 499.807 L 542.653 498.84 L 541.092 497.873 L 539.532 496.906 L 537.972 495.939 L 536.412 494.973 L 534.852 494.006 L 534.554 492.344 L 533.684 490.88 L 532.553 489.604 L 531.265 488.486 L 529.861 487.514 L 528.226 486.622 L 526.496 485.935 L 524.682 485.517 L 522.827 485.541 L 521.283 486.488 L 521.11 488.249 L 521.875 489.889 L 523.066 491.36 L 524.472 492.629 L 526.021 493.715 L 527.46 494.51 L 528.976 495.151 L 530.558 495.599 L 532.191 495.768 L 533.789 495.436 L 535.371 496.416 L 536.953 497.396 L 538.534 498.376 L 540.117 499.356 L 541.699 500.336 L 543.281 501.317 L 543.27 501.338 L 542.507 502.734 L 541.814 504.164 L 541.199 505.631 L 540.688 507.139 L 539.232 506.885 L 538 506.148 L 536.109 506.234 L 534.228 506.451 L 532.355 506.731 L 530.489 507.049 L 528.627 507.395 L 526.69 507.69 L 524.737 507.846 L 522.778 507.88 L 520.82 507.804 L 518.868 507.631 L 516.926 507.37 L 514.995 507.031 L 513.079 506.622 L 511.178 506.148 L 509.292 505.615 L 507.424 505.026 L 505.572 504.384 L 503.739 503.691 L 501.926 502.945 L 500.143 502.135 L 500.233 504.037 L 500.324 505.94 L 501.612 507.362 L 502.9 508.784 L 504.189 510.207 L 505.477 511.629 L 506.765 513.051 L 508.053 514.474 L 509.342 515.897 L 510.629 517.319 L 512.367 517.452 L 514.105 517.585 L 515.843 517.718 L 517.582 517.851 L 519.318 517.985 L 517.319 517.985 L 515.319 517.985 L 513.319 517.985 L 511.319 517.985 L 509.319 517.985 L 507.32 517.985 L 505.321 517.985 L 503.322 517.985 L 501.321 517.985 L 499.323 517.985 L 497.323 517.985 L 495.323 517.985 L 493.323 517.985 L 491.324 517.985 L 490.218 516.471 L 489.113 514.959 L 488.007 513.446 L 486.901 511.933 L 485.795 510.42 L 484.689 508.906 L 484.689 507.014 L 484.689 505.121 L 484.689 503.229 L 484.689 501.336 L 484.689 499.443 L 484.689 497.552 L 484.689 495.66 L 484.689 493.768 L 484.689 491.876 L 484.689 489.985 L 484.689 488.093 L 484.689 486.199 L 484.689 484.308 L 484.689 482.416 L 484.689 480.524 L 484.689 478.631 L 484.689 476.779 L 484.689 474.928 L 484.689 473.077 L 484.689 471.226 L 484.689 469.375 L 484.689 467.525 L 484.689 465.675 L 484.689 463.823 L 484.689 461.972 L 484.689 460.121 L 484.689 458.27 L 486.089 457.118 L 487.488 455.966 L 488.888 454.813 L 490.287 453.662 L 491.687 452.509 L 493.087 451.357 L 494.486 450.205 L 495.885 449.053 L 497.286 447.901 L 498.686 446.748 L 500.085 445.395 L 501.485 444.042 L 502.884 442.689 L 504.284 441.336 L 505.683 439.983 L 507.081 438.631 L 508.48 437.278 L 509.879 435.925 L 511.279 434.572 L 512.678 433.219 L 514.077 431.866 L 515.476 430.514 L 516.875 429.161 L 518.275 427.808 L 519.674 426.455 L 519.674 424.633 L 519.674 422.809 L 519.674 420.986 L 519.674 419.162 L 519.674 417.339 L 519.674 415.516 L 519.674 413.694 L 519.674 411.869 L 519.674 410.047 L 521.403 410.047 L 523.131 410.047 L 524.86 410.047 L 526.588 410.047 L 528.317 410.047 L 530.044 410.047 L 531.773 410.047 L 533.453 409.002 L 535.133 407.957 L 536.813 406.912 L 538.493 405.867 L 540.172 404.822 L 541.572 406.129 L 542.971 407.435 L 544.37 408.741 L 545.77 410.047 L 547.679 409.553 L 549.587 409.059 L 551.497 408.565 L 553.406 408.071 L 555.314 407.578 L 557.224 407.084 L 559.133 406.59 L 561.041 406.096 L 561.041 404.315 L 561.041 402.532 L 561.041 400.75 L 561.041 398.968 L 561.041 397.188 M 547.795 504.113 L 548.207 504.369 L 548.188 505.18 L 548.493 506.96 L 549.203 508.621 L 547.866 508.388 L 546.529 508.155 L 545.191 507.922 L 546.123 506.698 L 546.99 505.425 L 547.795 504.113 L 547.795 504.113 M 563.68 505.154 L 564.351 506.869 L 565.132 508.534 L 566.061 510.121 L 565.033 510.594 L 564.023 511.102 L 562.542 510.855 L 561.061 510.609 L 562.117 509.473 L 562.926 508.148 L 563.454 506.689 L 563.68 505.154 L 563.68 505.154)");
    Plotter plotter(5000, {0,1}, 1e-4);

    Lines lines(greenland.bounding_box(), 7.5, Vector(1./sqrt(2), 1./sqrt(2)));

    PlotCover cover(greenland, 10000, {0,1}, 1e-4);

    auto fill = plotter.fill(cover, lines);
    auto plot = plotter.plot(greenland);

    plot.insert(plot.end(), fill.begin(), fill.end());    

    auto stroke = simplify_plot(plot, false, 1e-4);


    
    
    auto mesh = mesher.create_mesh(stroke);



    std::cout << JSONAttributeMeshOutput(mesh) << std::endl;

    // for(size_t section = 0;; section++)
    // {
    //     auto s = mesh.submesh([section](size_t i, Point const & p, Point const & uv, Point const & brush, double const & sec) -> bool {
    //         return section == (size_t)sec;
    //     });

    //     if(s.size() == 0)
    //         break;

    //     std::cout << STLOutput(s.vertices()) << std::endl;


    // }


    // std::cout << OBJOutput(mesh.first);
    // for(auto const & m : mesh)
    //     std::cout << STLOutput(m);

    return 0;
}

