$( document ).ready(function() {
  
  const changeText = function (el, text, color) {
    el.text(text).css('color', color);
  };
  
  $('.input-1').keyup(function(){
    let len = this.value.length;
    const pbText = $('.form-1 .progress-bar_text');

    if (len === 0) {
      $('.form-1 .progress-bar_item').each(function() {
        $(this).removeClass('active')
      });
      $('.form-1 .active').css('background-color', 'transparent');
      changeText(pbText, 'Password is blank');
    } else if (len > 0 && len <= 4) {
      $('.form-1 .progress-bar_item-1').addClass('active');
      $('.form-1 .progress-bar_item-2').removeClass('active');
      $('.form-1 .progress-bar_item-3').removeClass('active');
      $('.form-1 .active').css('background-color', '#FF4B47');
      changeText(pbText, 'Too weak');
    } else if (len > 4 && len <= 8) {
      $('.form-1 .progress-bar_item-2').addClass('active');
      $('.form-1 .progress-bar_item-3').removeClass('active');
      $('.form-1 .active').css('background-color', '#F9AE35');
      changeText(pbText, 'Could be stronger');
    } else {
      $('.form-1 .progress-bar_item').each(function() {
        $(this).addClass('active');
      });
      $('.form-1 .active').css('background-color', '#2DAF7D');
      changeText(pbText, 'Strong password');
    } 
  });
  
  $('.input-2').keyup(function(){
    var pw = this.value;
    let len = pw.length;
    const pbText = $('.form-2 .progress-bar_text');
    var capitalletters = 0;
    var loweletters = 0;
    var number = 0;
    var upperCase= new RegExp('[A-Z]');
    var lowerCase= new RegExp('[a-z]');
    var numbers = new RegExp('[0-9]');
    if (pw.match(upperCase)) { capitalletters = 1} else { capitalletters = 0; }
    if (pw.match(lowerCase)) { loweletters = 1}  else { loweletters = 0; }
    if (pw.match(numbers)) { number = 1}  else { number = 0; }
    var total =  capitalletters + loweletters + number;
    console.log(len);
    console.log(capitalletters);
    console.log(loweletters);
    console.log(number);
    console.log(total);
    if (len === 0) {
      $('.form-2 .progress-bar_item').each(function() {
        $(this).removeClass('active')
      });
      changeText(pbText, 'Password is blank');
    } 
    else if (len > 0 && len <= 6) {
      $('.form-2 .progress-bar_item-1').addClass('active');
      $('.form-2 .progress-bar_item-2').removeClass('active');
      $('.form-2 .progress-bar_item-3').removeClass('active');
      changeText(pbText, 'Too weak');
    } 
    else if ((len > 6 && total <= 2) || len <= 9) {
      $('.form-2 .progress-bar_item-2').addClass('active');
      $('.form-2 .progress-bar_item-3').removeClass('active');
      changeText(pbText, 'Could be stronger');
    } else {
      $('.form-2 .progress-bar_item').each(function() {
        $(this).addClass('active');
      });
      changeText(pbText, 'Strong password');
    } 
  });
  
  $('.input-3').keyup(function(){
    let len = this.value.length;
    const pbText = $('.form-3 .progress-bar_text');

    if (len === 0) {
      $(this).css('border-bottom-color', '#2F96EF');
      changeText(pbText, 'Password is blank', '#aaa');
    } else if (len > 0 && len <= 4) {
      $(this).css('border-bottom-color', '#FF4B47');
      changeText(pbText, 'Too weak', '#FF4B47');
    } else if (len > 4 && len <= 8) {
      $(this).css('border-bottom-color', '#F9AE35');
      changeText(pbText, 'Could be stronger', '#aaa');
    } else {
      $(this).css('border-bottom-color', '#2DAF7D');
      changeText(pbText, 'Strong password');
    } 
  });
  
  $('.input-4').keyup(function(){
    let len = this.value.length;
    const pbText = $('.form-4 .progress-bar_text');

    if (len === 0) {
      $(this).css('border-color', '#2F96EF');
      changeText(pbText, 'Password is blank', '#aaa');
    } else if (len > 0 && len <= 4) {
      $(this).css('border-color', '#FF4B47');
      changeText(pbText, 'Too weak', '#FF4B47');
    } else if (len > 4 && len <= 8) {
      $(this).css('border-color', '#F9AE35');
      changeText(pbText, 'Could be stronger', '#F9AE35');
    } else {
      $(this).css('border-color', '#2DAF7D');
      changeText(pbText, 'Strong password', '#2DAF7D');
    } 
  });
  
});