import axios from "axios";

if (typeof window !== "undefined") {
  const community = localStorage.getItem('token');
    if (community) {
    axios.defaults.headers.common.Authorization = `Bearer ${community}`;
  }
}

axios.defaults.headers.common['Content-Type'] = 'application/json; charset=utf-8';
axios.defaults.baseURL = process.env.VUE_APP_URL;

export default axios;
